#include "SafeCIRBuilder.h"
#include "Utils.h"
#include <llvm-15/llvm/IR/Value.h>

void printType( llvm::Type *type );
// For lval and rval identification.
// Expect the function f to return an rval.
#define EXPECT_RVAL( f )                                                                 \
  val_type_stack.push( FLAG::RVAL );                                                     \
  f;                                                                                     \
  val_type_stack.pop( );
// Expect the function f to return an lval.
#define EXPECT_LVAL( f )                                                                 \
  val_type_stack.push( FLAG::LVAL );                                                     \
  f;                                                                                     \
  val_type_stack.pop( );
// Is current expected to return an lval.
#define IS_EXPECT_LVAL( ) val_type_stack.top( ) == FLAG::LVAL

SafeCIRBuilder::SafeCIRBuilder( llvm::LLVMContext &ctx ) : context( ctx ), builder( ctx )
{
}

SafeCIRBuilder::~SafeCIRBuilder( ) {}

void SafeCIRBuilder::obc_check( llvm::Value *index, int array_length, int node_line,
                                int node_pos, std::string name )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s %s %d %d %d\n", JJY_DEBUG_SIGN, __func__, name.c_str( ), node_line,
            node_pos, array_length );
  }
  // Basic logic:
  //          ... (current insert point)
  //          cmp = index < 0 || index >= length
  //          br cmp, check_fail, check_success
  //      check_fail bb:
  //          call obc_check(node_line, node_pos, name)
  //          ret void
  //      check_success bb:
  //          ... (next insert point here)

  // Create basic blocks
  llvm::Function *function = builder.GetInsertBlock( )->getParent( );
  llvm::BasicBlock *check_fail =
      llvm::BasicBlock::Create( context, "check_fail", function );
  llvm::BasicBlock *check_success =
      llvm::BasicBlock::Create( context, "check_success", function );

  // Insert code to check if index is in [0, length)
  llvm::Type *int32_type    = llvm::Type::getInt32Ty( context );
  llvm::Value *zero_const   = llvm::ConstantInt::get( int32_type, 0 );
  llvm::Value *length_const = llvm::ConstantInt::get( int32_type, array_length );
  llvm::Value *cmp_zero     = builder.CreateICmpSLT( index, zero_const );
  llvm::Value *cmp_length   = builder.CreateICmpSGE( index, length_const );
  llvm::Value *cmp          = builder.CreateOr( cmp_zero, cmp_length );
  builder.CreateCondBr( cmp, check_fail, check_success );

  // Insert code to check_fail
  builder.SetInsertPoint( check_fail );
  llvm::Value *arg0_val     = llvm::ConstantInt::get( int32_type, node_line );
  llvm::Value *arg1_val     = llvm::ConstantInt::get( int32_type, node_pos );
  llvm::Value *arg2_val     = llvm::ConstantDataArray::getString( context, name );
  llvm::Value *arg0_ptr     = lookup_variable( "arg0" ).val_ptr;
  llvm::Value *arg1_ptr     = lookup_variable( "arg1" ).val_ptr;
  llvm::Value *arg2_ptr     = lookup_variable( "arg2" ).val_ptr;
  llvm::Function *check_err = functions[ "obc_check_error" ];
  builder.CreateStore( arg0_val, arg0_ptr );
  builder.CreateStore( arg1_val, arg1_ptr );
  builder.CreateStore( arg2_val, arg2_ptr );
  builder.CreateCall( check_err, { } );
  builder.CreateRetVoid( );

  // Insert code to check_success
  builder.SetInsertPoint( check_success );

  return;
}

void SafeCIRBuilder::visit( comp_unit_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s comp_unit_node\n", JJY_DEBUG_SIGN, __func__ );
  }
  for ( auto comp_unit : node.comp_units ) {
    if ( auto global_var_decl =
             std::dynamic_pointer_cast<var_def_stmt_node>( comp_unit ) ) {

      cur_scope = FLAG::GLOBAL_SCOPE;
      global_var_decl->accept( *this );
    } else if ( auto global_func_def =
                    std::dynamic_pointer_cast<func_def_node>( comp_unit ) ) {
      global_func_def->accept( *this );
    }
  }
}

void SafeCIRBuilder::visit( comp_unit_child_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s comp_unit_child_node\n", JJY_DEBUG_SIGN, __func__ );
  }
  // Nothing to do here.
  return;
}

void SafeCIRBuilder::visit( global_def_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s global_def_node\n", JJY_DEBUG_SIGN, __func__ );
  }
  // Nothing to do here.
  return;
}

void SafeCIRBuilder::visit( func_def_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s func_def_node\n", JJY_DEBUG_SIGN, __func__ );
  }
  current_function = llvm::Function::Create(
      llvm::FunctionType::get( llvm::Type::getVoidTy( context ), { }, false ),
      llvm::GlobalValue::LinkageTypes::ExternalLinkage, node.name, module.get( ) );
  functions[ node.name ] = current_function;
  auto entry             = llvm::BasicBlock::Create( context, "entry", current_function );
  builder.SetInsertPoint( entry );
  bb_count  = 0;
  auto body = node.body;
  body->accept( *this );
  builder.CreateRetVoid( );
}

void SafeCIRBuilder::visit( block_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s block_node\n", JJY_DEBUG_SIGN, __func__ );
  }
  enter_scope( );
  for ( auto stmt : node.body ) {
    cur_scope = FLAG::BLOCK_SCOPE;
    if ( auto var_def_statement = std::dynamic_pointer_cast<var_def_stmt_node>( stmt ) ) {
      var_def_statement->accept( *this );
    } else if ( auto var_define = std::dynamic_pointer_cast<var_def_node>( stmt ) ) {
      var_define->accept( *this );
    } else if ( auto assign_statement =
                    std::dynamic_pointer_cast<assign_stmt_node>( stmt ) ) {
      assign_statement->accept( *this );
    } else if ( auto func_call_statement =
                    std::dynamic_pointer_cast<func_call_stmt_node>( stmt ) ) {
      func_call_statement->accept( *this );
    } else if ( auto block = std::dynamic_pointer_cast<block_node>( stmt ) ) {
      block->accept( *this );
    } else if ( auto if_statement = std::dynamic_pointer_cast<if_stmt_node>( stmt ) ) {
      if_statement->accept( *this );
    } else if ( auto while_statement =
                    std::dynamic_pointer_cast<while_stmt_node>( stmt ) ) {
      while_statement->accept( *this );
    } else if ( auto empty_statement =
                    std::dynamic_pointer_cast<empty_stmt_node>( stmt ) ) {
      empty_statement->accept( *this );
    }
    cur_scope = FLAG::GLOBAL_SCOPE;
  }
  exit_scope( );
}

void SafeCIRBuilder::visit( stmt_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s stmt_node\n", JJY_DEBUG_SIGN, __func__ );
  }
  // Nothing to do here.
}

void SafeCIRBuilder::visit( var_def_stmt_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s var_def_stmt_node\n", JJY_DEBUG_SIGN, __func__ );
  }
  for ( auto var_defs : node.var_defs ) {
    var_defs->accept( *this );
  }
}

void SafeCIRBuilder::visit( func_call_stmt_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s func_call_stmt_node\n", JJY_DEBUG_SIGN, __func__ );
  }
  if ( functions.count( node.name ) == 0 ) {
    std::cerr << node.line << ":" << node.pos << ": function " << node.name
              << " is not declared" << std::endl;
    error_flag = true;
    return;
  }
  builder.CreateCall( functions[ node.name ], { } );
}

void SafeCIRBuilder::visit( empty_stmt_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s empty_stmt_node\n", JJY_DEBUG_SIGN, __func__ );
  }
  // Nothing to do here.
}

void SafeCIRBuilder::visit( expr_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s expr_node\n", JJY_DEBUG_SIGN, __func__ );
  }
  // Nothing to do here.
}

void SafeCIRBuilder::visit( cond_node &node )
{
  // DONE: handle condition expression.
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s cond_node\n", JJY_DEBUG_SIGN, __func__ );
  }

  // define a lambda function to process the operand
  auto process_operand = [ & ]( ptr<expr_node> &node_operand, bool &is_int,
                                int &int_result, llvm::Value *&value_result ) {
    if ( !node_operand ) {
      std::cerr << node.line << ":" << node.pos << ": operand of condition is null"
                << std::endl;
      error_flag = true;
      return false;
    }
    EXPECT_RVAL( node_operand->accept( *this ) );
    int_result   = 0;
    is_int       = false;
    value_result = NULL;
    if ( !get_int_result( int_result ) ) {
      if ( !get_value_result( &value_result ) ) {
        std::cerr << node.line << ":" << node.pos
                  << ": operand of condition must be a constant or a variable"
                  << std::endl;
        error_flag = true;
        return false;
      }
    } else {
      is_int = true;
    }
    return true;
  };

  int lhs_int            = 0;
  int rhs_int            = 0;
  bool lhs_is_int        = false;
  bool rhs_is_int        = false;
  llvm::Value *lhs_value = NULL;
  llvm::Value *rhs_value = NULL;

  if ( !process_operand( node.lhs, lhs_is_int, lhs_int, lhs_value ) ) {
    return;
  }

  if ( !process_operand( node.rhs, rhs_is_int, rhs_int, rhs_value ) ) {
    return;
  }

  // create the comparison
  llvm::Value *cmp;
  llvm::Value *lhs = lhs_is_int ? builder.getInt32( lhs_int ) : lhs_value;
  llvm::Value *rhs = rhs_is_int ? builder.getInt32( rhs_int ) : rhs_value;

  switch ( node.op ) {
  case RelOp::LESS:
    cmp = builder.CreateICmpSLT( lhs, rhs );
    break;
  case RelOp::LESS_EQUAL:
    cmp = builder.CreateICmpSLE( lhs, rhs );
    break;
  case RelOp::GREATER:
    cmp = builder.CreateICmpSGT( lhs, rhs );
    break;
  case RelOp::GREATER_EQUAL:
    cmp = builder.CreateICmpSGE( lhs, rhs );
    break;
  case RelOp::EQUAL:
    cmp = builder.CreateICmpEQ( lhs, rhs );
    break;
  default:
    std::cerr << node.line << ":" << node.pos << ": unknown relation operator"
              << std::endl;
    error_flag = true;
    return;
  }
  set_value_result( cmp );
}

void SafeCIRBuilder::visit( number_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s number_node number = %d\n", JJY_DEBUG_SIGN, __func__, node.number );
  }
  set_int_result( node.number );
}

void SafeCIRBuilder::visit( binop_expr_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s binop_expr_node\n", JJY_DEBUG_SIGN, __func__ );
  }
  // DONE: handle binary operation.
  /*
      Visit lhs and rhs, there can be 2 cases:
      1. If lhs and rhs are both constant, we can calculate the result
     directly and provide a constant result to the parent node: for example:
     "1+2" -> set_int_result(1+2)
      2. If lhs and rhs are not both constant, build an instruction for the
     binary op: for example: "a+1" -> %res = add %a, 1; set_value_result(res)
          for example: "a+b" -> %res = add %a, %b; set_value_result(res)
  */
  int op1_int            = 0;
  int op2_int            = 0;
  bool op1_is_const      = false;
  bool op2_is_const      = false;
  llvm::Value *op1_value = NULL;
  llvm::Value *op2_value = NULL;

  node.lhs->accept( *this );
  if ( !get_int_result( op1_int ) ) {
    op1_is_const = false;
    if ( !get_value_result( &op1_value ) ) {
      std::cerr << node.line << ":" << node.pos
                << ": left operand of binary operator must be a constant or a "
                   "variable"
                << std::endl;
      error_flag = true;
      return;
    } else {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s binop_expr_node: op1 is not const\n", JJY_DEBUG_SIGN, __func__ );
      }
    }
  } else {
    op1_is_const = true;
    if ( JJY_DEBUG_IR ) {
      printf( "%s %s binop_expr_node: op1 is const = %d\n", JJY_DEBUG_SIGN, __func__,
              op1_int );
    }
  }

  node.rhs->accept( *this );
  if ( !get_int_result( op2_int ) ) {
    op2_is_const = false;
    if ( !get_value_result( &op2_value ) ) {
      std::cerr << node.line << ":" << node.pos
                << ": right operand of binary operator must be a constant or a "
                   "variable"
                << std::endl;
      error_flag = true;
      return;
    } else {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s binop_expr_node: op2 is not const\n", JJY_DEBUG_SIGN, __func__ );
      }
    }
  } else {
    op2_is_const = true;
    if ( JJY_DEBUG_IR ) {
      printf( "%s %s binop_expr_node: op2 is const = %d\n", JJY_DEBUG_SIGN, __func__,
              op2_int );
    }
  }

  if ( op1_is_const && op2_is_const ) {
    // both are constant
    switch ( node.op ) {
    case BinOp::PLUS:
      set_int_result( op1_int + op2_int );
      break;
    case BinOp::MINUS:
      set_int_result( op1_int - op2_int );
      break;
    case BinOp::MULTIPLY:
      set_int_result( op1_int * op2_int );
      break;
    case BinOp::DIVIDE:
      set_int_result( op1_int / op2_int );
      break;
    case BinOp::MODULO:
      set_int_result( op1_int % op2_int );
      break;
    default:
      std::cerr << node.line << ":" << node.pos << ": unknown binary operator"
                << std::endl;
      error_flag = true;
      return;
    }
  } else {
    // not both are constant
    //  2. If lhs and rhs are not both constant, build an instruction for
    //  the
    // binary op: for example: "a+1" -> %res = add %a, 1;
    // set_value_result(res)
    //      for example: "a+b" -> %res = add %a, %b; set_value_result(res)

    llvm::Value *res;
    switch ( node.op ) {
    case BinOp::PLUS:
      res = builder.CreateAdd( op1_is_const ? builder.getInt32( op1_int ) : op1_value,
                               op2_is_const ? builder.getInt32( op2_int ) : op2_value );
      break;
    case BinOp::MINUS:
      res = builder.CreateSub( op1_is_const ? builder.getInt32( op1_int ) : op1_value,
                               op2_is_const ? builder.getInt32( op2_int ) : op2_value );
      break;
    case BinOp::MULTIPLY:
      res = builder.CreateMul( op1_is_const ? builder.getInt32( op1_int ) : op1_value,
                               op2_is_const ? builder.getInt32( op2_int ) : op2_value );
      break;
    case BinOp::DIVIDE:
      res = builder.CreateSDiv( op1_is_const ? builder.getInt32( op1_int ) : op1_value,
                                op2_is_const ? builder.getInt32( op2_int ) : op2_value );
      break;
    case BinOp::MODULO:
      res = builder.CreateSRem( op1_is_const ? builder.getInt32( op1_int ) : op1_value,
                                op2_is_const ? builder.getInt32( op2_int ) : op2_value );
      break;
    default:
      std::cerr << node.line << ":" << node.pos << ": unknown binary operator"
                << std::endl;
      error_flag = true;
      return;
    }
    set_value_result( res );
  }
}

void SafeCIRBuilder::visit( unaryop_expr_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s unaryop_expr_node\n", JJY_DEBUG_SIGN, __func__ );
  }
  // TODO: handle unary operation.

  // struct unaryop_expr_node : expr_node {
  //     UnaryOp op;
  //     ptr<expr_node> rhs;
  //     virtual void accept(AstNode_Visitor& visitor) override;
  // };

  bool op1_is_const = false;
  int op1_int       = 0;
  llvm::Value *op1_value;

  node.rhs->accept( *this );
  if ( !get_int_result( op1_int ) ) {
    op1_is_const = false;
    if ( !get_value_result( &op1_value ) ) {
      std::cerr << node.line << ":" << node.pos
                << ": operand of unary operator must be a constant or a "
                   "variable"
                << std::endl;
      error_flag = true;
      return;
    } else {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s unaryop_expr_node: op1 is not const\n", JJY_DEBUG_SIGN, __func__ );
      }
    }
  } else {
    op1_is_const = true;
    if ( JJY_DEBUG_IR ) {
      printf( "%s %s unaryop_expr_node: op1 is const = %d\n", JJY_DEBUG_SIGN, __func__,
              op1_int );
    }
  }

  if ( op1_is_const ) {
    // op1 is constant
    switch ( node.op ) {
    case UnaryOp::MINUS:
      set_int_result( -op1_int );
      break;
    case UnaryOp::PLUS:
      set_int_result( op1_int );
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
    switch ( node.op ) {
    case UnaryOp::MINUS:
      set_value_result( builder.CreateNeg( op1_value ) );
      break;
    case UnaryOp::PLUS:
      set_value_result( op1_value );
      break;
    default:
      std::cerr << node.line << ":" << node.pos << ": unknown unary operator"
                << std::endl;
      error_flag = true;
      return;
    }
  }
}

void SafeCIRBuilder::visit( lval_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s lval_node\n", JJY_DEBUG_SIGN, __func__ );
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
  auto name        = node.name;
  VarInfo var_info = lookup_variable( name );
  if ( !var_info.is_valid( ) ) {
    std::cerr << node.line << ":" << node.pos << ": variable '" << name
              << "' is not declared" << std::endl;
    error_flag = true;
    return;
  }
  if ( JJY_DEBUG_IR ) {
    // name
    printf( "%s %s lval_node: name = %s\n", JJY_DEBUG_SIGN, __func__, name.c_str( ) );
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

  if ( !var_info.is_array ) {
    if ( JJY_DEBUG_IR ) {
      printf( "%s %s lval_node: not array\n", JJY_DEBUG_SIGN, __func__ );
    }
    // TODO: handle scalar lval

    if ( IS_EXPECT_LVAL( ) ) {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s lval_node: not array, expect lval\n", JJY_DEBUG_SIGN, __func__ );
      }
      set_value_result( var_info.val_ptr );
    } else {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s lval_node: not array, expect rval\n", JJY_DEBUG_SIGN, __func__ );
      }
      llvm::Type *type        = llvm::Type::getInt32Ty( context );
      llvm::Value *loaded_val = builder.CreateLoad( type, var_info.val_ptr, name );

      set_value_result( loaded_val );
    }

  } else {
    if ( JJY_DEBUG_IR ) {
      printf( "%s %s lval_node: is_array\n", JJY_DEBUG_SIGN, __func__ );
    }
    // TODO: handle array lval and call obc_check to insert obc check code
    // for obc array.

    if ( IS_EXPECT_LVAL( ) ) {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s lval_node: is_array, expect lval\n", JJY_DEBUG_SIGN, __func__ );
      }
      if ( !node.array_index ) {
        std::cerr << node.line << ":" << node.pos << ": array index must be provided"
                  << std::endl;
        error_flag = true;
        return;
      }
      node.array_index->accept( *this );
      int index_int;
      llvm::Value *index_value;
      if ( !get_int_result( index_int ) ) {
        if ( !get_value_result( &index_value ) ) {
          std::cerr << node.line << ":" << node.pos
                    << ": array index must be a constant or exp" << std::endl;
          error_flag = true;
          return;
        } else {
          if ( JJY_DEBUG_IR ) {
            printf( "%s %s lval_node: is_array, expect lval, index "
                    "is variable\n",
                    JJY_DEBUG_SIGN, __func__ );
          }
          // index is a variable
          // TODO:
          // check if index is in [0, length)
          // insert the call

          // load the index_value
          llvm::Value *index_value_load =
              builder.CreateLoad( llvm::Type::getInt32Ty( context ), index_value, "" );
          llvm::Value *index_value_load_const = builder.CreateIntCast(
              index_value_load, llvm::Type::getInt32Ty( context ), true );

          if ( var_info.is_obc ) {
            // llvm::Value *index_value_for_check =
            // builder.CreateLoad(
            //     llvm::Type::getInt32Ty(context), index_value,
            //     name);
            obc_check( index_value_load_const, var_info.array_length, node.line, node.pos,
                       name );
          }

          llvm::Value *element_ptr = builder.CreateGEP(
              llvm::ArrayType::get( llvm::Type::getInt32Ty( context ),
                                    var_info.array_length ),
              var_info.val_ptr, { builder.getInt32( 0 ), index_value_load_const } );
          // need to get the value of the element

          set_value_result( element_ptr );
        }
      } else {
        // index is a constant
        // if (index_int < 0 || index_int >= var_info.array_length) {
        //   // std::cerr << node.line << ":" << node.pos
        //   //           << ": array index out of bound" << std::endl;
        //   error_flag = true;
        //   return;
        // } else {

        // index is in [0, length)

        // get array element
        llvm::Value *index_value =
            llvm::ConstantInt::get( llvm::Type::getInt32Ty( context ), index_int );
        if ( var_info.is_obc ) {
          obc_check( index_value, var_info.array_length, node.line, node.pos, name );
        }
        llvm::Value *element_ptr =
            builder.CreateGEP( llvm::ArrayType::get( llvm::Type::getInt32Ty( context ),
                                                     var_info.array_length ),
                               var_info.val_ptr, { builder.getInt32( 0 ), index_value } );
        // need to get the value of the element
        //
        set_value_result( element_ptr );
        // }
      }
    } else {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s lval_node: is_array, expect rval\n", JJY_DEBUG_SIGN, __func__ );
      }
      if ( !node.array_index ) {
        std::cerr << node.line << ":" << node.pos << ": array index must be provided"
                  << std::endl;
        error_flag = true;
        return;
      }
      node.array_index->accept( *this );
      int index_int;
      llvm::Value *index_value;
      if ( !get_int_result( index_int ) ) {
        if ( !get_value_result( &index_value ) ) {
          std::cerr << node.line << ":" << node.pos
                    << ": array index must be a constant or exp" << std::endl;
          error_flag = true;
          return;
        } else {
          if ( JJY_DEBUG_IR ) {
            printf( "%s %s lval_node: is_array, expect rval, index "
                    "is variable\n",
                    JJY_DEBUG_SIGN, __func__ );
          }
          // index is a variable
          // TODO:
          // check if index is in [0, length)
          // insert the call

          // load the index_value
          if ( var_info.is_obc ) {
            // llvm::Value *index_value_for_check =
            // builder.CreateLoad(
            //     llvm::Type::getInt32Ty(context), index_value,
            //     name);
            obc_check( index_value, var_info.array_length, node.line, node.pos, name );
          }
          // XXXX: 下面的不对
          // llvm::Value *index_val = builder.CreateLoad(
          //     llvm::Type::getInt32Ty(context), index_value, name);
          // get array element
          llvm::Value *element_ptr = builder.CreateGEP(
              llvm::ArrayType::get( llvm::Type::getInt32Ty( context ),
                                    var_info.array_length ),
              var_info.val_ptr, { builder.getInt32( 0 ), index_value } );
          // need to get the value of the element

          llvm::Value *element_value =
              builder.CreateLoad( llvm::Type::getInt32Ty( context ), element_ptr, name );
          set_value_result( element_value );
        }
      } else {
        // index is a constant
        // if (index_int < 0 || index_int >= var_info.array_length) {
        //   // Runtime Error: array 'x' out of bound check error at
        //   Line:6 Pos:5 std::cerr << node.line << ":" << node.pos
        //             << ": array index out of bound" << std::endl;
        //   // std::cerr << "Runtime Error: array '" << name
        //   //           << "' out of bound check error at Line:" <<
        //   node.line
        //   //           << " Pos:" << node.pos << std::endl;
        //   error_flag = true;
        //   return;
        // } else {

        // index is in [0, length)

        // get array element
        llvm::Value *index_value =
            llvm::ConstantInt::get( llvm::Type::getInt32Ty( context ), index_int );
        if ( var_info.is_obc ) {
          obc_check( index_value, var_info.array_length, node.line, node.pos, name );
        }
        llvm::Value *element_ptr =
            builder.CreateGEP( llvm::ArrayType::get( llvm::Type::getInt32Ty( context ),
                                                     var_info.array_length ),
                               var_info.val_ptr, { builder.getInt32( 0 ), index_value } );
        // need to get the value of the element
        //
        llvm::Value *element_value =
            builder.CreateLoad( llvm::Type::getInt32Ty( context ), element_ptr, name );
        set_value_result( element_value );
      }
    }
    return;
  }
}

void SafeCIRBuilder::visit( var_def_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s var_def_node\n", JJY_DEBUG_SIGN, __func__ );
  }
  std::string name                   = node.name;
  bool is_const                      = node.is_const;
  bool is_obc                        = node.is_obc;
  ptr<expr_node> array_length        = node.array_length;
  ptr_vector<expr_node> initializers = node.initializers;

  VarInfo var_info                   = lookup_variable( name );
  size_t scope_index;
  for ( auto it = scoped_variables.rbegin( ); it != scoped_variables.rend( ); ++it ) {
    if ( it->variable_map.count( name ) ) {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s var_def_node: found %s\n", JJY_DEBUG_SIGN, __func__,
                name.c_str( ) );
      }
      scope_index = std::distance( it, scoped_variables.rend( ) );
      break;
    }
  }

  if ( JJY_DEBUG_IR ) {
    printf( "%s %s cur_scope = %d scope_index = %ld\n", JJY_DEBUG_SIGN, __func__,
            cur_scope, scope_index );
  }
  // 同一层作用域
  if ( var_info.is_valid( ) && scoped_variables.size( ) == scope_index ) {
    std::cerr << node.line << ":" << node.pos << ": variable '" << name
              << "' is declared more than one times" << std::endl;
    error_flag = true;
    return;
  }

  if ( cur_scope == FLAG::GLOBAL_SCOPE ) { // global define
    llvm::GlobalVariable *global_variable;

    if ( !array_length ) {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s var_def_node: global not array\n", JJY_DEBUG_SIGN, __func__ );
      }
      // not an global array
      // TODO: create and declare global scalar
      global_variable = new llvm::GlobalVariable(
          *module, llvm::Type::getInt32Ty( context ), is_const,
          llvm::GlobalValue::LinkageTypes::ExternalLinkage, nullptr, name );

      if ( initializers.size( ) > 0 ) {
        int init_int;
        EXPECT_LVAL( initializers[ 0 ]->accept( *this ) );
        if ( !get_int_result( init_int ) ) {
          std::cerr << node.line << ":" << node.pos << ": initializer must be a constant"
                    << std::endl;
          error_flag = true;
          return;
        }
        global_variable->setInitializer(
            llvm::ConstantInt::get( llvm::Type::getInt32Ty( context ), init_int ) );
      } else {
        global_variable->setInitializer(
            llvm::ConstantInt::get( llvm::Type::getInt32Ty( context ), 0 ) );
      }
      declare_variable( name, global_variable, is_const, false, is_obc, 0 );
    } else {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s var_def_node: global is array\n", JJY_DEBUG_SIGN, __func__ );
      }
      // is an global array
      // TODO: create and declare global array

      // get array length
      // XXXX: length can be a expression
      int length;
      array_length->accept( *this );
      if ( !get_int_result( length ) ) {
        std::cerr << node.line << ":" << node.pos << ": array length must be a constant"
                  << std::endl;
        error_flag = true;
        return;
      }
      // Initialize
      if ( initializers.size( ) > length ) {
        std::cerr << node.line << ":" << node.pos
                  << ": excess elements in the initializer of array '" << name << "'"
                  << std::endl;
        error_flag = true;
        return;
      } else if ( length <= 0 ) {
        std::cerr << node.line << ":" << node.pos << ": size of array '" << name
                  << "' is not positive" << std::endl;
        error_flag = true;
        return;
      } else {
        if ( JJY_DEBUG_IR ) {
          printf( "%s %s var_def_node: global is array, init size = "
                  "%ld\n",
                  JJY_DEBUG_SIGN, __func__, initializers.size( ) );
        }
        std::vector<llvm::Constant *> global_array_init_values;
        for ( size_t i = 0; i < length; ++i ) {
          // llvm::Value *index =
          //     llvm::ConstantInt::get(llvm::Type::getInt32Ty(context),
          //     i);
          // llvm::Value *element_ptr = builder.CreateGEP(
          //     array_type, global_variable, {builder.getInt32(0),
          //     index});
          if ( i < initializers.size( ) ) {
            int init_int;
            llvm::Value *init_value;
            EXPECT_RVAL( initializers[ i ]->accept( *this ) );
            if ( !get_int_result( init_int ) ) {
              if ( !get_value_result( &init_value ) ) {
                std::cerr << node.line << ":" << node.pos
                          << ": array initializer must be a "
                             "constant or a variable"
                          << std::endl;
                error_flag = true;
                return;
              } else {
                if ( JJY_DEBUG_IR ) {
                  printf( "%s %s var_def_node: global is "
                          "array, init size = "
                          "%ld, init is variable\n",
                          JJY_DEBUG_SIGN, __func__, initializers.size( ) );
                }
              }
            } else {
              if ( JJY_DEBUG_IR ) {
                printf( "%s %s var_def_node: global is array, "
                        "init size = %ld, "
                        "init is const = %d\n",
                        JJY_DEBUG_SIGN, __func__, initializers.size( ), init_int );
              }
              global_array_init_values.push_back(
                  llvm::ConstantInt::get( context, llvm::APInt( 32, init_int ) ) );
            }
          } else {
            if ( JJY_DEBUG_IR ) {
              printf( "%s %s var_def_node: global is array, init "
                      "size = %ld, "
                      "init is 0\n",
                      JJY_DEBUG_SIGN, __func__, initializers.size( ) );
            }
            global_array_init_values.push_back(
                llvm::ConstantInt::get( context, llvm::APInt( 32, 0 ) ) );
          }
        }
        llvm::ArrayRef<llvm::Constant *> global_array_ref( global_array_init_values );
        llvm::Constant *global_array_init_const = llvm::ConstantArray::get(
            llvm::ArrayType::get( llvm::Type::getInt32Ty( context ), length ),
            global_array_ref );

        // auto res = new GlobalVariable(*module, builder.getInt32Ty(),
        // false,
        //                               GlobalValue::LinkageTypes::ExternalLinkage,
        //                               res_init_val_array, "res");

        // global_variable = new llvm::GlobalVariable(
        //     *module, builder.getInt32Ty(), is_const,
        //     llvm::GlobalValue::LinkageTypes::ExternalLinkage,
        //     nullptr, name);
        // 创建全局变量res
        // 参数：模块，类型，是否常量，链接类型，初始化值，变量名
        global_variable =
            new llvm::GlobalVariable( *module, builder.getInt32Ty( ), is_const,
                                      llvm::GlobalValue::LinkageTypes::ExternalLinkage,
                                      global_array_init_const, name );
        declare_variable( name, global_variable, is_const, true, is_obc, length );
        // std::vector<llvm::Constant *> res_init_values;
        // for (int i = 0; i < 5; i++) {
        //   res_init_values.push_back(builder.getInt32(i));
        // }
        // llvm::ArrayRef<llvm::Constant *>
        // res_arrayref(res_init_values); auto res_init_val_array =
        // llvm::ConstantArray::get(
        //     llvm::ArrayType::get(llvm::Type::getInt32Ty(context), 5),
        //     res_arrayref);
        // // 创建全局变量res
        // // 参数：模块，类型，是否常量，链接类型，初始化值，变量名
        // auto res = new GlobalVariable(*module, builder.getInt32Ty(),
        // false,
        //                               GlobalValue::LinkageTypes::ExternalLinkage,
        //                               res_init_val_array, "res");
      }
    }
  } else { // local define
    llvm::AllocaInst *local_variable;

    if ( !array_length ) {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s var_def_node: local not array\n", JJY_DEBUG_SIGN, __func__ );
      }
      // not an local array
      // TODO: create and declare local scalar
      local_variable =
          builder.CreateAlloca( llvm::Type::getInt32Ty( context ), nullptr, name );
      if ( initializers.size( ) > 0 ) {
        int init_int;
        llvm::Value *init_value;
        EXPECT_RVAL( initializers[ 0 ]->accept( *this ) );
        if ( !get_int_result( init_int ) ) {
          if ( !get_value_result( &init_value ) ) {
            std::cerr << node.line << ":" << node.pos
                      << ": initializer must be a constant or a exp" << std::endl;
            error_flag = true;
            return;
          } else {
            builder.CreateStore( init_value, local_variable );
          }
        } else {
          if ( JJY_DEBUG_IR ) {
            printf( "%s %s var_def_node: local not array, init is "
                    "const = %d\n",
                    JJY_DEBUG_SIGN, __func__, init_int );
          }
          builder.CreateStore(
              llvm::ConstantInt::get( context, llvm::APInt( 32, init_int ) ),
              local_variable );
        }
      }
      declare_variable( name, local_variable, is_const, false, is_obc, 0 );
    } else {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s var_def_node: local is array\n", JJY_DEBUG_SIGN, __func__ );
      }
      // is an array
      // TODO: create and declare local array

      // get array length

      int length;
      // XXXX: length can be a expression
      array_length->accept( *this );
      if ( !get_int_result( length ) ) {
        std::cerr << node.line << ":" << node.pos << ": array length must be a constant"
                  << std::endl;
        error_flag = true;
        return;
      }
      llvm::ArrayType *array_type =
          llvm::ArrayType::get( llvm::Type::getInt32Ty( context ), length );
      local_variable = builder.CreateAlloca( array_type, nullptr, name );

      // Initialize
      if ( initializers.size( ) > length ) {
        std::cerr << node.line << ":" << node.pos
                  << ": excess elements in the initializer of array '" << name << "'"
                  << std::endl;
        error_flag = true;
        return;
      } else if ( length <= 0 ) {
        std::cerr << node.line << ":" << node.pos << ": size of array '" << name
                  << "' is not positive" << std::endl;
        error_flag = true;
        return;
      } else {
        if ( JJY_DEBUG_IR ) {
          printf( "%s %s var_def_node: local is array, init size = %ld\n", JJY_DEBUG_SIGN,
                  __func__, initializers.size( ) );
        }
        for ( size_t i = 0; i < length; ++i ) {
          llvm::Value *index =
              llvm::ConstantInt::get( llvm::Type::getInt32Ty( context ), i );
          llvm::Value *element_ptr = builder.CreateGEP(
              array_type, local_variable, { builder.getInt32( 0 ), index } );
          if ( i < initializers.size( ) ) {
            int init_int;
            llvm::Value *init_value;
            EXPECT_RVAL( initializers[ i ]->accept( *this ) );
            if ( !get_int_result( init_int ) ) {
              if ( !get_value_result( &init_value ) ) {
                std::cerr << node.line << ":" << node.pos
                          << ": array initializer must be a "
                             "constant or a "
                             "variable"
                          << std::endl;
                error_flag = true;
                return;
              } else {
                builder.CreateStore( init_value, element_ptr );
              }
            } else {
              builder.CreateStore( builder.getInt32( init_int ), element_ptr );
            }
          } else {
            builder.CreateStore( builder.getInt32( 0 ), element_ptr );
          }
        }
        declare_variable( name, local_variable, is_const, true, is_obc, length );
      }
    }
  }
}

void SafeCIRBuilder::visit( assign_stmt_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s assign_stmt_node\n", JJY_DEBUG_SIGN, __func__ );
  }
  // TODO: get target's rval and store at value's lval.
  //
  // struct assign_stmt_node : stmt_node {
  //     ptr<lval_node> target;
  //     ptr<expr_node> value;
  //     virtual void accept(AstNode_Visitor& visitor) override;
  // };

  EXPECT_LVAL( node.target->accept( *this ) );
  llvm::Value *target;
  if ( !get_value_result( &target ) ) {
    std::cerr << node.line << ":" << node.pos
              << ": left operand of assignment must be a variable" << std::endl;
    error_flag = true;
    return;
  } else {
    VarInfo var_info = lookup_variable( node.target->name );
    if ( JJY_DEBUG_IR ) {
      printf( "%s %s target name=%s is_const=%d\n", JJY_DEBUG_SIGN, __func__,
              node.target->name.c_str( ), var_info.is_const );
    }
    if ( var_info.is_const == true ) {
      std::cerr << node.line << ":" << node.pos << ": assignment of read-only variable '"
                << node.target->name << "'" << std::endl;
      error_flag = true;
      return;
    }
  }

  EXPECT_RVAL( node.value->accept( *this ) );
  llvm::Value *assign_value;
  int assign_int;
  // expr_node
  if ( !get_int_result( assign_int ) ) {
    if ( !get_value_result( &assign_value ) ) {
      std::cerr << node.line << ":" << node.pos
                << ": right operand of assignment must be a constant or a "
                   "variable"
                << std::endl;
      error_flag = true;
      return;
    } else {
    }
  } else {
    assign_value = builder.getInt32( assign_int );
  }

  builder.CreateStore( assign_value, target );
}

void SafeCIRBuilder::visit( if_stmt_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s if_stmt_node\n", JJY_DEBUG_SIGN, __func__ );
  }
  // struct if_stmt_node : stmt_node {
  //   ptr<cond_node> cond;
  //   ptr<stmt_node> if_body;
  //   ptr<stmt_node> else_body;
  //   virtual void accept(AstNode_Visitor &visitor) override;
  // };
  // TODO: implement if-else statement.

  // Visit condition
  node.cond->accept( *this );

  llvm::Value *cond_value;
  if ( !get_value_result( &cond_value ) ) {
    std::cerr << node.line << ":" << node.pos
              << ": condition of if statement must be a variable" << std::endl;
    error_flag = true;
    return;
  }

  // Create blocks for if and else
  llvm::Function *function      = builder.GetInsertBlock( )->getParent( );
  llvm::BasicBlock *if_block    = llvm::BasicBlock::Create( context, "if", function );
  llvm::BasicBlock *else_block  = llvm::BasicBlock::Create( context, "else", function );
  llvm::BasicBlock *merge_block = llvm::BasicBlock::Create( context, "merge", function );

  // Create conditional branch
  builder.CreateCondBr( cond_value, if_block, else_block );

  // Emit if block
  builder.SetInsertPoint( if_block );
  node.if_body->accept( *this );
  builder.CreateBr( merge_block );

  // Emit else block
  builder.SetInsertPoint( else_block );
  if ( node.else_body ) {
    node.else_body->accept( *this );
  }

  builder.CreateBr( merge_block );

  // Emit merge block
  builder.SetInsertPoint( merge_block );
}

void SafeCIRBuilder::visit( while_stmt_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s while_stmt_node\n", JJY_DEBUG_SIGN, __func__ );
  }
  // struct while_stmt_node : stmt_node {
  //     ptr<cond_node> cond;
  //     ptr<stmt_node> body;
  //     virtual void accept(AstNode_Visitor& visitor) override;
  // };
  // TODO: implement while statement.

  // Create blocks for while loop
  llvm::Function *function      = builder.GetInsertBlock( )->getParent( );
  llvm::BasicBlock *cond_block  = llvm::BasicBlock::Create( context, "cond", function );
  llvm::BasicBlock *body_block  = llvm::BasicBlock::Create( context, "body", function );
  llvm::BasicBlock *merge_block = llvm::BasicBlock::Create( context, "merge", function );

  // Create unconditional branch to condition block
  builder.CreateBr( cond_block );

  // Emit condition block
  builder.SetInsertPoint( cond_block );
  node.cond->accept( *this );

  llvm::Value *cond_value;
  if ( !get_value_result( &cond_value ) ) {
    std::cerr << node.line << ":" << node.pos
              << ": condition of while statement must be a variable" << std::endl;
    error_flag = true;
    return;
  }

  // Create conditional branch
  builder.CreateCondBr( cond_value, body_block, merge_block );

  // Emit body block
  builder.SetInsertPoint( body_block );
  node.body->accept( *this );
  builder.CreateBr( cond_block );

  // Emit merge block
  builder.SetInsertPoint( merge_block );

  // XXXX: 退出循环时，应该清除循环内部的变量
}

void SafeCIRBuilder::log( std::string info )
{
  if ( debug ) {
    std::cerr << info << std::endl;
  }
};

void SafeCIRBuilder::build( std::string name, std::shared_ptr<ast_node> node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s\n", JJY_DEBUG_SIGN, __func__ );
  }
  // Initialize environment.
  module  = std::make_unique<llvm::Module>( name, context );
  runtime = std::make_unique<runtime_info>( module.get( ) );

  // global
  enter_scope( );
  for ( auto t : runtime->get_language_symbols( ) ) {
    llvm::GlobalValue *val;
    std::string name;
    bool is_function;
    bool is_const;
    bool is_array;
    bool is_obc;
    int array_length;
    std::tie( name, val, is_function, is_const, is_array, is_obc, array_length ) = t;
    if ( is_function )
      functions[ name ] = static_cast<llvm::Function *>( val );
    else
      declare_variable( name, val, is_const, is_array, is_obc, array_length );
  }

  cur_scope  = FLAG::GLOBAL_SCOPE;
  error_flag = false;

  // Start building by starting iterate over the syntax tree.
  node->accept( *this );

  // Finish by clear IRBuilder's insertion point and moving away built module.
  builder.ClearInsertionPoint( );
  exit_scope( );

  if ( error_flag ) {
    module.release( );
    runtime.release( );
  }
}

std::unique_ptr<llvm::Module> SafeCIRBuilder::get_module( )
{
  return std::move( module );
}

std::unique_ptr<runtime_info> SafeCIRBuilder::get_runtime_info( )
{
  return std::move( runtime );
}

void SafeCIRBuilder::enter_scope( )
{
  scoped_variables.emplace_back( );
  scoped_variables.back( ).scope_type = cur_scope;
  cur_scope                           = FLAG::BLOCK_SCOPE;
}

void SafeCIRBuilder::exit_scope( )
{
  scoped_variables.pop_back( );
  cur_scope = scoped_variables.back( ).scope_type;
}

SafeCIRBuilder::VarInfo SafeCIRBuilder::lookup_variable( std::string name )
{
  if ( JJY_DEBUG_IR ) {
    // print the scoped_variables
    printf( "%s %s %ld\n", JJY_DEBUG_SIGN, __func__, scoped_variables.size( ) );
  }
  // TODO: find the nearest decalred variable `name`

  for ( auto it = scoped_variables.rbegin( ); it != scoped_variables.rend( ); ++it ) {
    if ( it->variable_map.count( name ) ) {
      return it->variable_map[ name ];
    }
  }

  return VarInfo( ); // Return an invalid VarInfo if `name` not found.
}

// 在当前作用域声明一个变量
bool SafeCIRBuilder::declare_variable( std::string name, llvm::Value *var_ptr,
                                       bool is_const, bool is_array, bool is_obc,
                                       int array_length )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s\n", JJY_DEBUG_SIGN, __func__ );
  }
  if ( scoped_variables.back( ).variable_map.count( name ) )
    return false;
  scoped_variables.back( ).variable_map[ name ] =
      VarInfo( var_ptr, is_const, is_array, is_obc, array_length );
  return true;
}

// 把某个值存到int_result中，并设置has_int_result为true
void SafeCIRBuilder::set_int_result( int val )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s\n", JJY_DEBUG_SIGN, __func__ );
  }
  has_int_result = true;
  int_result     = val;
}

// 从int_result中取值，如果has_int_result为true，则返回true，否则返回false
bool SafeCIRBuilder::get_int_result( int &val )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s\n", JJY_DEBUG_SIGN, __func__ );
  }
  if ( has_int_result ) {
    val            = int_result;
    has_int_result = false;
    return true;
  } else {
    return false;
  }
}

// 把某个值存到value_result中，并设置has_value_result为true
void SafeCIRBuilder::set_value_result( llvm::Value *val )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s\n", JJY_DEBUG_SIGN, __func__ );
  }
  has_value_result = true;
  value_result     = val;
}

// 从value_result中取值，如果has_value_result为true，则返回true，否则返回false
bool SafeCIRBuilder::get_value_result( llvm::Value **val )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s\n", JJY_DEBUG_SIGN, __func__ );
  }
  if ( has_value_result ) {
    *val             = value_result;
    has_value_result = false;
    return true;
  } else {
    return false;
  }
}

// 取value result或int result，如果value result存在，则返回true, 或者int
// result存在，则返回true，否则返回false
bool SafeCIRBuilder::get_result_as_value( llvm::Value **val )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s\n", JJY_DEBUG_SIGN, __func__ );
  }
  if ( has_value_result ) {
    *val             = value_result;
    has_value_result = false;
    return true;
  } else if ( has_int_result ) {
    *val           = builder.getInt32( int_result );
    has_int_result = false;
    return true;
  } else {
    return false;
  }
}
void printType( llvm::Type *type )
{
  std::string type_str;
  llvm::raw_string_ostream rso( type_str );
  type->print( rso );
  std::cout << "Type: " << rso.str( ) << std::endl;
}
