#include "SafeCIRBuilder.h"
#include "Utils.h"
#include <llvm-15/llvm/IR/Value.h>
void printType(llvm::Type *type);
// For lval and rval identification.
// Expect the function f to return an rval.
// 期望f返回一个右值类型
#define EXPECT_RVAL(f)                                                         \
  val_type_stack.push(FLAG::RVAL);                                             \
  f;                                                                           \
  val_type_stack.pop();
// Expect the function f to return an lval.
// 期望f返回一个左值类型
#define EXPECT_LVAL(f)                                                         \
  val_type_stack.push(FLAG::LVAL);                                             \
  f;                                                                           \
  val_type_stack.pop();
// Is current expected to return an lval.
// 当前期望返回一个左值类型
#define IS_EXPECT_LVAL() val_type_stack.top() == FLAG::LVAL

SafeCIRBuilder::SafeCIRBuilder(llvm::LLVMContext &ctx)
    : context(ctx), builder(ctx) {}

SafeCIRBuilder::~SafeCIRBuilder() {}

void SafeCIRBuilder::obc_check(llvm::Value *index, int array_length,
                               int node_line, int node_pos, std::string name) {
  if (JJY_DEBUG_IR) {
    printf("%s %s %s %d %d %d\n", JJY_DEBUG_SIGN, __func__, name.c_str(),
           node_line, node_pos, array_length);
  }
  // Insert code to check if index is in [0, length).
  // If not in range, call obc_check_error to report error.
  // Basic logic:
  //          ... (current insert point)
  //          cmp = index < 0 || index >= length
  //          br cmp, check_fail, check_success
  //      check_fail bb:
  //          call obc_check(node_line, node_pos, name)
  //          ret void
  //      check_success bb:
  //          ... (next insert point here)

  // TODO: Implement.

  // Create basic blocks
  llvm::Function *function = builder.GetInsertBlock()->getParent();
  llvm::BasicBlock *check_fail_bb =
      llvm::BasicBlock::Create(context, "check_fail", function);
  llvm::BasicBlock *check_success_bb =
      llvm::BasicBlock::Create(context, "check_success", function);

  // Create cmp
  llvm::Value *cmp = builder.CreateOr(
      builder.CreateICmpSLT(index, builder.getInt32(0)),
      builder.CreateICmpSGE(index, builder.getInt32(array_length)));

  // Create br
  builder.CreateCondBr(cmp, check_fail_bb, check_success_bb);

  // Insert code to check_fail_bb
  // check_fail function should print
  // "Runtime Error: array '%s' out of bound check error at Line:%d Pos:%d"
  function->getBasicBlockList().push_back(check_fail_bb);
  builder.SetInsertPoint(check_fail_bb);
  llvm::Function *check_err = functions["obc_check_error"];
  llvm::Value *arg0_val =
      llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), node_line);
  llvm::Value *arg1_val =
      llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), node_pos);
  llvm::Value *arg2_val = llvm::ConstantDataArray::getString(context, name);
  llvm::Value *arg0_ptr = lookup_variable("arg0").val_ptr;
  llvm::Value *arg1_ptr = lookup_variable("arg1").val_ptr;
  llvm::Value *arg2_ptr = lookup_variable("arg2").val_ptr;
  builder.CreateStore(arg0_val, arg0_ptr);
  builder.CreateStore(arg1_val, arg1_ptr);
  builder.CreateStore(arg2_val, arg2_ptr);
  // Create a format string for the error message
  llvm::Value *format_str = llvm::ConstantDataArray::getString(
      context,
      "Runtime Error: array '%s' out of bound check error at Line:%d Pos:%d");

  // Create a global variable to hold the format string
  llvm::GlobalVariable *format_str_var = new llvm::GlobalVariable(
      *module, format_str->getType(), true, llvm::GlobalValue::PrivateLinkage,
      llvm::cast<llvm::Constant>(format_str), "format_str");

  // Get a pointer to the format string
  llvm::Value *format_str_ptr = builder.CreatePointerCast(
      format_str_var, llvm::Type::getInt8PtrTy(context));

  // Create a call to printf with the format string and arguments
  llvm::Function *printf_func = module->getFunction("printf");
  // if (!printf_func) {
  //   llvm::FunctionType *printf_type =
  //       llvm::FunctionType::get(llvm::Type::getInt32Ty(context),
  //                               llvm::Type::getInt8PtrTy(context), true);

  //   llvm::Function *printf_func = llvm::Function::Create(
  //       printf_type, llvm::Function::ExternalLinkage, 0, "printf", module);
  // }
  builder.CreateCall(printf_func,
                     {format_str_ptr, arg2_val, arg0_val, arg1_val});
  builder.CreateCall(check_err, {});
  builder.CreateRetVoid();

  // Insert code to check_success_bb
  function->getBasicBlockList().push_back(check_success_bb);
  builder.SetInsertPoint(check_success_bb);

  /*
      // Call obc_check_error to report error:
      llvm::Value *arg0_val =
     llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), node_line);
      llvm::Value *arg1_val =
     llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), node_pos);
      llvm::Value *arg2_val = llvm::ConstantDataArray::getString(context, name);
      llvm::Value *arg0_ptr = lookup_variable("arg0").val_ptr;
      llvm::Value *arg1_ptr = lookup_variable("arg1").val_ptr;
      llvm::Value *arg2_ptr = lookup_variable("arg2").val_ptr;
      llvm::Function *check_err = functions["obc_check_error"];
      builder.CreateStore(arg0_val, arg0_ptr);
      builder.CreateStore(arg1_val, arg1_ptr);
      builder.CreateStore(arg2_val, arg2_ptr);
      builder.CreateCall(check_err, {});
  */

  return;
}

void SafeCIRBuilder::visit(comp_unit_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s comp_unit_node\n", JJY_DEBUG_SIGN, __func__);
  }
  for (auto comp_unit : node.comp_units) {
    if (auto global_var_decl =
            std::dynamic_pointer_cast<var_def_stmt_node>(comp_unit)) {

      cur_scope = FLAG::GLOBAL_SCOPE;
      global_var_decl->accept(*this);

    } else if (auto global_func_def =
                   std::dynamic_pointer_cast<func_def_node>(comp_unit)) {

      global_func_def->accept(*this);
    }
  }
}

void SafeCIRBuilder::visit(comp_unit_child_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s comp_unit_child_node\n", JJY_DEBUG_SIGN, __func__);
  }
  // Nothing to do here.
  return;
}

void SafeCIRBuilder::visit(global_def_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s global_def_node\n", JJY_DEBUG_SIGN, __func__);
  }
  // Nothing to do here.
  return;
}

void SafeCIRBuilder::visit(func_def_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s func_def_node\n", JJY_DEBUG_SIGN, __func__);
  }
  current_function = llvm::Function::Create(
      llvm::FunctionType::get(llvm::Type::getVoidTy(context), {}, false),
      llvm::GlobalValue::LinkageTypes::ExternalLinkage, node.name,
      module.get());
  functions[node.name] = current_function;
  auto entry = llvm::BasicBlock::Create(context, "entry", current_function);
  builder.SetInsertPoint(entry);
  bb_count = 0;
  auto body = node.body;
  body->accept(*this);
  builder.CreateRetVoid();
}

void SafeCIRBuilder::visit(block_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s block_node\n", JJY_DEBUG_SIGN, __func__);
  }
  enter_scope();
  for (auto stmt : node.body) {
    cur_scope = FLAG::BLOCK_SCOPE;
    if (auto var_def_statement =
            std::dynamic_pointer_cast<var_def_stmt_node>(stmt)) {
      var_def_statement->accept(*this);
    } else if (auto var_define =
                   std::dynamic_pointer_cast<var_def_node>(stmt)) {
      var_define->accept(*this);
    } else if (auto assign_statement =
                   std::dynamic_pointer_cast<assign_stmt_node>(stmt)) {
      assign_statement->accept(*this);
    } else if (auto func_call_statement =
                   std::dynamic_pointer_cast<func_call_stmt_node>(stmt)) {
      func_call_statement->accept(*this);
    } else if (auto block = std::dynamic_pointer_cast<block_node>(stmt)) {
      block->accept(*this);
    } else if (auto if_statement =
                   std::dynamic_pointer_cast<if_stmt_node>(stmt)) {
      if_statement->accept(*this);
    } else if (auto while_statement =
                   std::dynamic_pointer_cast<while_stmt_node>(stmt)) {
      while_statement->accept(*this);
    } else if (auto empty_statement =
                   std::dynamic_pointer_cast<empty_stmt_node>(stmt)) {
      empty_statement->accept(*this);
    }
    cur_scope = FLAG::GLOBAL_SCOPE;
  }
  exit_scope();
}

void SafeCIRBuilder::visit(stmt_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s stmt_node\n", JJY_DEBUG_SIGN, __func__);
  }
  // Nothing to do here.
}

void SafeCIRBuilder::visit(var_def_stmt_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s var_def_stmt_node\n", JJY_DEBUG_SIGN, __func__);
  }
  for (auto var_defs : node.var_defs) {
    var_defs->accept(*this);
  }
}

void SafeCIRBuilder::visit(func_call_stmt_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s func_call_stmt_node\n", JJY_DEBUG_SIGN, __func__);
  }
  if (functions.count(node.name) == 0) {
    std::cerr << node.line << ":" << node.pos << ": function " << node.name
              << " is not declared" << std::endl;
    error_flag = true;
    return;
  }
  builder.CreateCall(functions[node.name], {});
}

void SafeCIRBuilder::visit(empty_stmt_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s empty_stmt_node\n", JJY_DEBUG_SIGN, __func__);
  }
  // Nothing to do here.
}

void SafeCIRBuilder::visit(expr_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s expr_node\n", JJY_DEBUG_SIGN, __func__);
  }
  // Nothing to do here.
}

void SafeCIRBuilder::visit(cond_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s cond_node\n", JJY_DEBUG_SIGN, __func__);
  }
  // TODO: handle condition expression.
}

void SafeCIRBuilder::visit(number_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s number_node\n", JJY_DEBUG_SIGN, __func__);
  }
  set_int_result(node.number);
}

void SafeCIRBuilder::visit(binop_expr_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s binop_expr_node\n", JJY_DEBUG_SIGN, __func__);
  }
  // TODO: handle binary operation.
  /*
      Visit lhs and rhs, there can be 2 cases:
      1. If lhs and rhs are both constant, we can calculate the result directly
     and provide a constant result to the parent node: for example: "1+2" ->
     set_int_result(1+2)
      2. If lhs and rhs are not both constant, build an instruction for the
     binary op: for example: "a+1" -> %res = add %a, 1; set_value_result(res)
          for example: "a+b" -> %res = add %a, %b; set_value_result(res)
  */
  bool op1_is_const = false;
  bool op2_is_const = false;
  int op1_int = 0;
  int op2_int = 0;
  llvm::Value *op1_value;
  llvm::Value *op2_value;

  node.lhs->accept(*this);
  if (!get_int_result(op1_int)) {
    op1_is_const = false;
    if (!get_value_result(&op1_value)) {
      std::cerr << node.line << ":" << node.pos
                << ": left operand of binary operator must be a constant or a "
                   "variable"
                << std::endl;
      error_flag = true;
      return;
    } else {
      if (JJY_DEBUG_IR) {
        printf("%s %s binop_expr_node: op1 is not const\n", JJY_DEBUG_SIGN,
               __func__);
      }
    }
  } else {
    op1_is_const = true;
    if (JJY_DEBUG_IR) {
      printf("%s %s binop_expr_node: op1 is const = %d\n", JJY_DEBUG_SIGN,
             __func__, op1_int);
    }
  }

  node.rhs->accept(*this);
  if (!get_int_result(op2_int)) {
    op2_is_const = false;
    if (!get_value_result(&op2_value)) {
      std::cerr << node.line << ":" << node.pos
                << ": right operand of binary operator must be a constant or a "
                   "variable"
                << std::endl;
      error_flag = true;
      return;
    } else {
      if (JJY_DEBUG_IR) {
        printf("%s %s binop_expr_node: op2 is not const\n", JJY_DEBUG_SIGN,
               __func__);
      }
    }
  } else {
    op2_is_const = true;
    if (JJY_DEBUG_IR) {
      printf("%s %s binop_expr_node: op2 is const = %d\n", JJY_DEBUG_SIGN,
             __func__, op2_int);
    }
  }

  if (op1_is_const && op2_is_const) {
    // both are constant
    switch (node.op) {
    case BinOp::PLUS:
      set_int_result(op1_int + op2_int);
      break;
    case BinOp::MINUS:
      set_int_result(op1_int - op2_int);
      break;
    case BinOp::MULTIPLY:
      set_int_result(op1_int * op2_int);
      break;
    case BinOp::DIVIDE:
      set_int_result(op1_int / op2_int);
      break;
    case BinOp::MODULO:
      set_int_result(op1_int % op2_int);
      break;
    default:
      std::cerr << node.line << ":" << node.pos << ": unknown binary operator"
                << std::endl;
      error_flag = true;
      return;
    }
  } else {
    // not both are constant
    //  2. If lhs and rhs are not both constant, build an instruction for the
    // binary op: for example: "a+1" -> %res = add %a, 1; set_value_result(res)
    //      for example: "a+b" -> %res = add %a, %b; set_value_result(res)

    llvm::Value *res;
    switch (node.op) {
    case BinOp::PLUS:
      res = builder.CreateAdd(
          op1_is_const ? builder.getInt32(op1_int) : op1_value,
          op2_is_const ? builder.getInt32(op2_int) : op2_value);
      break;
    case BinOp::MINUS:
      res = builder.CreateSub(
          op1_is_const ? builder.getInt32(op1_int) : op1_value,
          op2_is_const ? builder.getInt32(op2_int) : op2_value);
      break;
    case BinOp::MULTIPLY:
      res = builder.CreateMul(
          op1_is_const ? builder.getInt32(op1_int) : op1_value,
          op2_is_const ? builder.getInt32(op2_int) : op2_value);
      break;
    case BinOp::DIVIDE:
      res = builder.CreateSDiv(
          op1_is_const ? builder.getInt32(op1_int) : op1_value,
          op2_is_const ? builder.getInt32(op2_int) : op2_value);
      break;
    case BinOp::MODULO:
      res = builder.CreateSRem(
          op1_is_const ? builder.getInt32(op1_int) : op1_value,
          op2_is_const ? builder.getInt32(op2_int) : op2_value);
      break;
    default:
      std::cerr << node.line << ":" << node.pos << ": unknown binary operator"
                << std::endl;
      error_flag = true;
      return;
    }
  }
}

void SafeCIRBuilder::visit(unaryop_expr_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s unaryop_expr_node\n", JJY_DEBUG_SIGN, __func__);
  }
  // TODO: handle unary operation.

  // struct unaryop_expr_node : expr_node {
  //     UnaryOp op;
  //     ptr<expr_node> rhs;
  //     virtual void accept(AstNode_Visitor& visitor) override;
  // };

  bool op1_is_const = false;
  int op1_int = 0;
  llvm::Value *op1_value;

  node.rhs->accept(*this);
  if (!get_int_result(op1_int)) {
    op1_is_const = false;
    if (!get_value_result(&op1_value)) {
      std::cerr << node.line << ":" << node.pos
                << ": operand of unary operator must be a constant or a "
                   "variable"
                << std::endl;
      error_flag = true;
      return;
    } else {
      if (JJY_DEBUG_IR) {
        printf("%s %s unaryop_expr_node: op1 is not const\n", JJY_DEBUG_SIGN,
               __func__);
      }
    }
  } else {
    op1_is_const = true;
    if (JJY_DEBUG_IR) {
      printf("%s %s unaryop_expr_node: op1 is const = %d\n", JJY_DEBUG_SIGN,
             __func__, op1_int);
    }
  }

  if (op1_is_const) {
    // op1 is constant
    switch (node.op) {
    case UnaryOp::MINUS:
      set_int_result(-op1_int);
      break;
    case UnaryOp::PLUS:
      set_int_result(op1_int);
      break;
    default:
      std::cerr << node.line << ":" << node.pos << ": unknown unary operator"
                << std::endl;
      error_flag = true;
      return;
    }
  } else {
    // op1 is not constant

    llvm::Value *res;
    switch (node.op) {
    case UnaryOp::MINUS:
      set_value_result(builder.CreateNeg(op1_value));
      break;
    case UnaryOp::PLUS:
      set_value_result(op1_value);
      break;
    default:
      std::cerr << node.line << ":" << node.pos << ": unknown unary operator"
                << std::endl;
      error_flag = true;
      return;
    }
  }
}

void SafeCIRBuilder::visit(lval_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s lval_node\n", JJY_DEBUG_SIGN, __func__);
  }
  // struct VarInfo {
  //     llvm::Value* val_ptr;
  //     bool is_const;
  //     bool is_array;
  //     bool is_obc;
  //     int array_length;
  //     VarInfo(llvm::Value* val_ptr, bool is_const, bool is_array, bool
  //     is_obc, int array_length)
  //         : val_ptr(val_ptr), is_const(is_const), is_array(is_array),
  //         is_obc(is_obc),
  //           array_length(array_length) {};
  //     VarInfo() : val_ptr(nullptr), is_const(false), is_array(false),
  //     is_obc(false),
  //                 array_length(0) {};
  //     bool is_valid() { return val_ptr != nullptr; };
  // };
  auto name = node.name;
  VarInfo var_info = lookup_variable(name);
  if (!var_info.is_valid()) {
    std::cerr << node.line << ":" << node.pos << ": variable '" << name
              << "' is not declared" << std::endl;
    error_flag = true;
    return;
  }
  if (JJY_DEBUG_IR) {
    // name
    printf("%s %s lval_node: name = %s\n", JJY_DEBUG_SIGN, __func__,
           name.c_str());
  }

  /*
      Use IS_EXPECT_LVAL() to check if the parent node expects an lval or an
     rval. And the parent must use EXPECT_LVAL() or EXPECT_RVAL() properly. If
     lval is expected, we can directly return the lval of the variable: for
     example: in "%a = 1" and we are visiting "%a", we can return the
     llvm::Value* of %a. If rval is expected, we need to load the value of the
     variable and return the loaded value. for example: in "1 + *%a" and we
     are visiting "%a", we need to return the loaded value of %a, create "%tmp
     = load %a" and then set_value_result(%tmp).
  */

  if (!var_info.is_array) {
    if (JJY_DEBUG_IR) {
      printf("%s %s lval_node: not array\n", JJY_DEBUG_SIGN, __func__);
    }
    // TODO: handle scalar lval

    if (IS_EXPECT_LVAL()) {
      if (JJY_DEBUG_IR) {
        printf("%s %s lval_node: not array, expect lval\n", JJY_DEBUG_SIGN,
               __func__);
        printType(var_info.val_ptr->getType());
      }
      set_value_result(var_info.val_ptr);

    } else {
      llvm::Type *type = var_info.val_ptr->getType();
      llvm::Value *loaded_val =
          builder.CreateLoad(type, var_info.val_ptr, name);
      set_value_result(loaded_val);
    }

  } else {
    if (JJY_DEBUG_IR) {
      printf("%s %s lval_node: is_array\n", JJY_DEBUG_SIGN, __func__);
    }
    // TODO: handle array lval and call obc_check to insert obc check code for
    // obc array.

    if (IS_EXPECT_LVAL()) {
      if (JJY_DEBUG_IR) {
        printf("%s %s lval_node: is_array, expect lval\n", JJY_DEBUG_SIGN,
               __func__);
      }
      // this is an array need index
      // node.array_index
      if (!node.array_index) {
        std::cerr << node.line << ":" << node.pos
                  << ": array index must be provided" << std::endl;
        error_flag = true;
        return;
      }
      node.array_index->accept(*this);
      int index;
      llvm::Value *index_value;
      if (!get_int_result(index)) {
        if (!get_value_result(&index_value)) {
          std::cerr << node.line << ":" << node.pos
                    << ": array index must be a constant or exp" << std::endl;
          error_flag = true;
          return;
        } else {
          if (JJY_DEBUG_IR) {
            printf(
                "%s %s lval_node: is_array, expect lval, index is variable\n",
                JJY_DEBUG_SIGN, __func__);
          }
          // index is a variable
          // TODO:
          // check if index is in [0, length)
          // insert the call
          obc_check(index_value, var_info.array_length, node.line, node.pos,
                    name);

          // load the index_value
          llvm::Value *index_val = builder.CreateLoad(
              llvm::Type::getInt32Ty(context), index_value, name);
          // get array element
          llvm::Value *element_ptr = builder.CreateGEP(
              llvm::ArrayType::get(llvm::Type::getInt32Ty(context),
                                   var_info.array_length),
              var_info.val_ptr, {builder.getInt32(0), index_val});
          // need to get the value of the element

          llvm::Value *element_value = builder.CreateLoad(
              llvm::Type::getInt32Ty(context), element_ptr, name);
          set_value_result(element_value);
        }
      } else {
        // index is a constant
        if (index < 0 || index >= var_info.array_length) {
          std::cerr << node.line << ":" << node.pos
                    << ": array index out of bound" << std::endl;
          error_flag = true;
          return;
        } else {
          // index is in [0, length)
          obc_check(index_value, var_info.array_length, node.line, node.pos,
                    name);

          // get array element
          llvm::Value *index_val =
              llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), index);
          llvm::Value *element_ptr = builder.CreateGEP(
              llvm::ArrayType::get(llvm::Type::getInt32Ty(context),
                                   var_info.array_length),
              var_info.val_ptr, {builder.getInt32(0), index_val});
          // need to get the value of the element
          //
          llvm::Value *element_value = builder.CreateLoad(
              llvm::Type::getInt32Ty(context), element_ptr, name);
          set_value_result(element_value);
        }
      }

    } else {
    }
    return;
  }
}

void SafeCIRBuilder::visit(var_def_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s var_def_node\n", JJY_DEBUG_SIGN, __func__);
  }
  std::string name = node.name;
  bool is_const = node.is_const;
  bool is_obc = node.is_obc;
  ptr<expr_node> array_length = node.array_length;
  ptr_vector<expr_node> initializers = node.initializers;

  if (cur_scope == FLAG::GLOBAL_SCOPE) { // global define
    llvm::GlobalVariable *global_variable;

    if (!array_length) {
      if (JJY_DEBUG_IR) {
        printf("%s %s var_def_node: global not array\n", JJY_DEBUG_SIGN,
               __func__);
      }
      // not an array
      // TODO: create and declare global scalar
      global_variable = new llvm::GlobalVariable(
          *module, llvm::Type::getInt32Ty(context), is_const,
          llvm::GlobalValue::LinkageTypes::ExternalLinkage, nullptr, name);
      declare_variable(name, global_variable, is_const, false, is_obc, 0);
    } else {
      if (JJY_DEBUG_IR) {
        printf("%s %s var_def_node: global is array\n", JJY_DEBUG_SIGN,
               __func__);
      }
      // is an array
      // TODO: create and declare global array
    }
  } else { // local define
    llvm::Value *local_variable;

    if (!array_length) {
      if (JJY_DEBUG_IR) {
        printf("%s %s var_def_node: local not array\n", JJY_DEBUG_SIGN,
               __func__);
      }
      // not an array
      // TODO: create and declare local scalar
    } else {
      if (JJY_DEBUG_IR) {
        printf("%s %s var_def_node: local is array\n", JJY_DEBUG_SIGN,
               __func__);
      }
      // is an array
      // TODO: create and declare local array

      // get array length

      int length;
      // XXXX: length can be a expression
      array_length->accept(*this);
      if (!get_int_result(length)) {
        std::cerr << node.line << ":" << node.pos
                  << ": array length must be a constant" << std::endl;
        error_flag = true;
        return;
      }
      llvm::ArrayType *array_type =
          llvm::ArrayType::get(llvm::Type::getInt32Ty(context), length);
      local_variable = builder.CreateAlloca(array_type, nullptr, name);

      // Initialize
      if (initializers.size() > length) {
        std::cerr << node.line << ":" << node.pos
                  << ": too many initializers for array '" << name << "'"
                  << std::endl;
        error_flag = true;
        return;
      } else {
        if (JJY_DEBUG_IR) {
          printf("%s %s var_def_node: local is array, init size = %ld\n",
                 JJY_DEBUG_SIGN, __func__, initializers.size());
        }
        for (size_t i = 0; i < length; ++i) {
          if (i < initializers.size()) {
            int init_value;
            initializers[i]->accept(*this);
            if (!get_int_result(init_value)) {
              std::cerr << node.line << ":" << node.pos
                        << ": array initializer must be a constant"
                        << std::endl;
              error_flag = true;
              return;
            }
            llvm::Value *index =
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), i);
            llvm::Value *element_ptr = builder.CreateGEP(
                array_type, local_variable, {builder.getInt32(0), index});
            builder.CreateStore(builder.getInt32(init_value), element_ptr);
          } else {
            llvm::Value *index =
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), i);
            llvm::Value *element_ptr = builder.CreateGEP(
                array_type, local_variable, {builder.getInt32(0), index});
            builder.CreateStore(builder.getInt32(0), element_ptr);
          }
        }
        declare_variable(name, local_variable, is_const, true, is_obc, length);
      }
    }
  }
}

void SafeCIRBuilder::visit(assign_stmt_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s assign_stmt_node\n", JJY_DEBUG_SIGN, __func__);
  }
  // TODO: get target's rval and store at value's lval.
  //
  // struct assign_stmt_node : stmt_node {
  //     ptr<lval_node> target;
  //     ptr<expr_node> value;
  //     virtual void accept(AstNode_Visitor& visitor) override;
  // };

  EXPECT_LVAL(node.target->accept(*this));
  llvm::Value *target;
  if (!get_value_result(&target)) {
    std::cerr << node.line << ":" << node.pos
              << ": left operand of assignment must be a variable" << std::endl;
    error_flag = true;
    return;
  }

  EXPECT_LVAL(node.value->accept(*this));

  llvm::Value *value;
  // expr_node
  if (!get_value_result(&value)) {
    std::cerr << node.line << ":" << node.pos
              << ": right operand of assignment must be a constant or a "
                 "variable"
              << std::endl;
    error_flag = true;
    return;
  }

  builder.CreateStore(value, target);
}

void SafeCIRBuilder::visit(if_stmt_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s if_stmt_node\n", JJY_DEBUG_SIGN, __func__);
  }
  // TODO: implement if-else statement.
}

void SafeCIRBuilder::visit(while_stmt_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s while_stmt_node\n", JJY_DEBUG_SIGN, __func__);
  }
  // TODO: implement while statement.
}

void SafeCIRBuilder::log(std::string info) {
  if (debug) {
    std::cerr << info << std::endl;
  }
};

void SafeCIRBuilder::build(std::string name, std::shared_ptr<ast_node> node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s\n", JJY_DEBUG_SIGN, __func__);
  }
  // Initialize environment.
  module = std::make_unique<llvm::Module>(name, context);
  runtime = std::make_unique<runtime_info>(module.get());

  // global
  enter_scope();
  for (auto t : runtime->get_language_symbols()) {
    llvm::GlobalValue *val;
    std::string name;
    bool is_function;
    bool is_const;
    bool is_array;
    bool is_obc;
    int array_length;
    std::tie(name, val, is_function, is_const, is_array, is_obc, array_length) =
        t;
    if (is_function)
      functions[name] = static_cast<llvm::Function *>(val);
    else
      declare_variable(name, val, is_const, is_array, is_obc, array_length);
  }

  cur_scope = FLAG::GLOBAL_SCOPE;
  error_flag = false;

  // Start building by starting iterate over the syntax tree.
  node->accept(*this);

  // Finish by clear IRBuilder's insertion point and moving away built module.
  builder.ClearInsertionPoint();
  exit_scope();

  if (error_flag) {
    module.release();
    runtime.release();
  }
}

std::unique_ptr<llvm::Module> SafeCIRBuilder::get_module() {
  return std::move(module);
}

std::unique_ptr<runtime_info> SafeCIRBuilder::get_runtime_info() {
  return std::move(runtime);
}

void SafeCIRBuilder::enter_scope() {
  scoped_variables.emplace_back();
  scoped_variables.back().scope_type = cur_scope;
  cur_scope = FLAG::BLOCK_SCOPE;
}

void SafeCIRBuilder::exit_scope() {
  scoped_variables.pop_back();
  cur_scope = scoped_variables.back().scope_type;
}

SafeCIRBuilder::VarInfo SafeCIRBuilder::lookup_variable(std::string name) {
  if (JJY_DEBUG_IR) {
    printf("%s %s\n", JJY_DEBUG_SIGN, __func__);
  }
  // TODO: find the nearest decalred variable `name`

  for (auto it = scoped_variables.rbegin(); it != scoped_variables.rend();
       ++it) {
    if (it->variable_map.count(name)) {
      return it->variable_map[name];
    }
  }

  return VarInfo(); // Return an invalid VarInfo if `name` not found.
}

// 在当前作用域声明一个变量
bool SafeCIRBuilder::declare_variable(std::string name, llvm::Value *var_ptr,
                                      bool is_const, bool is_array, bool is_obc,
                                      int array_length) {
  if (JJY_DEBUG_IR) {
    printf("%s %s\n", JJY_DEBUG_SIGN, __func__);
  }
  if (scoped_variables.back().variable_map.count(name))
    return false;
  scoped_variables.back().variable_map[name] =
      VarInfo(var_ptr, is_const, is_array, is_obc, array_length);
  return true;
}

// 把某个值存到int_result中，并设置has_int_result为true
void SafeCIRBuilder::set_int_result(int val) {
  if (JJY_DEBUG_IR) {
    printf("%s %s\n", JJY_DEBUG_SIGN, __func__);
  }
  has_int_result = true;
  int_result = val;
}

// 从int_result中取值，如果has_int_result为true，则返回true，否则返回false
bool SafeCIRBuilder::get_int_result(int &val) {
  if (JJY_DEBUG_IR) {
    printf("%s %s\n", JJY_DEBUG_SIGN, __func__);
  }
  if (has_int_result) {
    val = int_result;
    has_int_result = false;
    return true;
  } else {
    return false;
  }
}

// 把某个值存到value_result中，并设置has_value_result为true
void SafeCIRBuilder::set_value_result(llvm::Value *val) {
  if (JJY_DEBUG_IR) {
    printf("%s %s\n", JJY_DEBUG_SIGN, __func__);
  }
  has_value_result = true;
  value_result = val;
}

// 从value_result中取值，如果has_value_result为true，则返回true，否则返回false
bool SafeCIRBuilder::get_value_result(llvm::Value **val) {
  if (JJY_DEBUG_IR) {
    printf("%s %s\n", JJY_DEBUG_SIGN, __func__);
  }
  if (has_value_result) {
    *val = value_result;
    has_value_result = false;
    return true;
  } else {
    return false;
  }
}

// 取value result或int result，如果value result存在，则返回true, 或者int
// result存在，则返回true，否则返回false
bool SafeCIRBuilder::get_result_as_value(llvm::Value **val) {
  if (JJY_DEBUG_IR) {
    printf("%s %s\n", JJY_DEBUG_SIGN, __func__);
  }
  if (has_value_result) {
    *val = value_result;
    has_value_result = false;
    return true;
  } else if (has_int_result) {
    *val = builder.getInt32(int_result);
    has_int_result = false;
    return true;
  } else {
    return false;
  }
}
void printType(llvm::Type *type) {
  std::string type_str;
  llvm::raw_string_ostream rso(type_str);
  type->print(rso);
  std::cout << "Type: " << rso.str() << std::endl;
}
