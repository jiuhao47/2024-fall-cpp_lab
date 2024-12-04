#include "SafeCIRBuilder.h"
#include "Utils.h"

// For lval and rval identification.
// Expect the function f to return an rval.
#define EXPECT_RVAL(f)                                                         \
  val_type_stack.push(FLAG::RVAL);                                             \
  f;                                                                           \
  val_type_stack.pop();
// Expect the function f to return an lval.
#define EXPECT_LVAL(f)                                                         \
  val_type_stack.push(FLAG::LVAL);                                             \
  f;                                                                           \
  val_type_stack.pop();
// Is current expected to return an lval.
#define IS_EXPECT_LVAL() val_type_stack.top() == FLAG::LVAL

SafeCIRBuilder::SafeCIRBuilder(llvm::LLVMContext &ctx)
    : context(ctx), builder(ctx) {}

SafeCIRBuilder::~SafeCIRBuilder() {}

void SafeCIRBuilder::obc_check(llvm::Value *index, int array_length,
                               int node_line, int node_pos, std::string name) {
  if (JJY_DEBUG_IR) {
    printf("%s %s", JJY_DEBUG_SIGN, __func__);
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
}

void SafeCIRBuilder::visit(unaryop_expr_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s unaryop_expr_node\n", JJY_DEBUG_SIGN, __func__);
  }
  // TODO: handle unary operation.
}

void SafeCIRBuilder::visit(lval_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s lval_node\n", JJY_DEBUG_SIGN, __func__);
  }
  auto name = node.name;
  VarInfo var_info = lookup_variable(name);
  if (!var_info.is_valid()) {
    std::cerr << node.line << ":" << node.pos << ": variable '" << name
              << "' is not declared" << std::endl;
    error_flag = true;
    return;
  }

  /*
      Use IS_EXPECT_LVAL() to check if the parent node expects an lval or an
     rval. And the parent must use EXPECT_LVAL() or EXPECT_RVAL() properly. If
     lval is expected, we can directly return the lval of the variable: for
     example: in "%a = 1" and we are visiting "%a", we can return the
     llvm::Value* of %a. If rval is expected, we need to load the value of the
     variable and return the loaded value. for example: in "1 + *%a" and we are
     visiting "%a", we need to return the loaded value of %a, create "%tmp =
     load %a" and then set_value_result(%tmp).
  */

  if (!var_info.is_array) {
    if (JJY_DEBUG_IR) {
      printf("%s %s lval_node: not array\n", JJY_DEBUG_SIGN, __func__);
    }
    // TODO: handle scalar lval
  } else {
    if (JJY_DEBUG_IR) {
      printf("%s %s lval_node: is_array\n", JJY_DEBUG_SIGN, __func__);
    }
    // TODO: handle array lval and call obc_check to insert obc check code for
    // obc array.
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

    if (!array_length) { // not an array
                         // TODO: create and declare global scalar
    } else {             // is an array
                         // TODO: create and declare global array
    }
  } else { // local define
    llvm::Value *local_variable;

    if (!array_length) { // not an array
                         // TODO: create and declare local scalar
    } else {             // is an array
                         // TODO: create and declare local array
    }
  }
}

void SafeCIRBuilder::visit(assign_stmt_node &node) {
  if (JJY_DEBUG_IR) {
    printf("%s %s assign_stmt_node\n", JJY_DEBUG_SIGN, __func__);
  }
  // TODO: get target's rval and store at value's lval.
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

  return VarInfo(); // Return an invalid VarInfo if `name` not found.
}

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

void SafeCIRBuilder::set_int_result(int val) {
  if (JJY_DEBUG_IR) {
    printf("%s %s\n", JJY_DEBUG_SIGN, __func__);
  }
  has_int_result = true;
  int_result = val;
}

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

void SafeCIRBuilder::set_value_result(llvm::Value *val) {
  if (JJY_DEBUG_IR) {
    printf("%s %s\n", JJY_DEBUG_SIGN, __func__);
  }
  has_value_result = true;
  value_result = val;
}

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
