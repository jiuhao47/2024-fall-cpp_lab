#include "SafeCIRBuilder.h"
#include "Utils.h"
#include <llvm-15/llvm/IR/Value.h>

// For obc_check count
int obc_check_count = 0;

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
  std::string name_suffix  = std::to_string( obc_check_count );
  llvm::Function *function = builder.GetInsertBlock( )->getParent( );
  llvm::BasicBlock *obc_check =
      llvm::BasicBlock::Create( context, "obc_check_" + name_suffix, function );
  llvm::BasicBlock *obc_err =
      llvm::BasicBlock::Create( context, "obc_err_" + name_suffix, function );
  llvm::BasicBlock *obc_ok =
      llvm::BasicBlock::Create( context, "obc_ok_" + name_suffix, function );

  // Create no conditional jump to obc_check
  builder.CreateBr( obc_check );
  builder.SetInsertPoint( obc_check );

  // Insert code to check if index is in [0, length)
  llvm::Type *int32_type    = llvm::Type::getInt32Ty( context );
  llvm::Value *zero_const   = llvm::ConstantInt::get( int32_type, 0 );
  llvm::Value *length_const = llvm::ConstantInt::get( int32_type, array_length );
  llvm::Value *cmp_zero     = builder.CreateICmpSLT( index, zero_const );
  llvm::Value *cmp_length   = builder.CreateICmpSGE( index, length_const );
  llvm::Value *cmp          = builder.CreateOr( cmp_length, cmp_zero );
  builder.CreateCondBr( cmp, obc_err, obc_ok );

  // Insert code to check_fail
  builder.SetInsertPoint( obc_err );
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
  builder.SetInsertPoint( obc_ok );

  obc_check_count++;
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

  // define a lambda function to process the operand
  auto process_operand = [ & ]( ptr<expr_node> &node_operand, bool &is_int,
                                int &int_result, llvm::Value *&value_result,
                                const std::string &operand_position ) {
    if ( !node_operand ) {
      std::cerr << node.line << ":" << node.pos << ": " << operand_position
                << " operand of condition is null" << std::endl;
      error_flag = true;
      return false;
    }
    node_operand->accept( *this );
    int_result   = 0;
    is_int       = false;
    value_result = NULL;
    if ( !get_int_result( int_result ) ) {
      if ( !get_value_result( &value_result ) ) {
        std::cerr << node.line << ":" << node.pos << ": " << operand_position
                  << " operand of condition must be a constant or a variable"
                  << std::endl;
        error_flag = true;
        return false;
      } else {
        // op is not const
        // nothing to do
      }
    } else {
      is_int = true;
    }
    return true;
  };

  int op1_int            = 0;
  int op2_int            = 0;
  bool op1_is_int        = false;
  bool op2_is_int        = false;
  llvm::Value *op1_value = NULL;
  llvm::Value *op2_value = NULL;

  if ( !process_operand( node.lhs, op1_is_int, op1_int, op1_value, "left" ) ) {
    return;
  }

  if ( !process_operand( node.rhs, op2_is_int, op2_int, op2_value, "right" ) ) {
    return;
  }

  if ( op1_is_int && op2_is_int ) {

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
    llvm::Value *binary_instr;
    llvm::Value *op1 = op1_is_int ? builder.getInt32( op1_int ) : op1_value;
    llvm::Value *op2 = op2_is_int ? builder.getInt32( op2_int ) : op2_value;

    switch ( node.op ) {
    case BinOp::PLUS:
      binary_instr = builder.CreateAdd( op1, op2 );
      break;
    case BinOp::MINUS:
      binary_instr = builder.CreateSub( op1, op2 );
      break;
    case BinOp::MULTIPLY:
      binary_instr = builder.CreateMul( op1, op2 );
      break;
    case BinOp::DIVIDE:
      binary_instr = builder.CreateSDiv( op1, op2 );
      break;
    case BinOp::MODULO:
      binary_instr = builder.CreateSRem( op1, op2 );
      break;
    default:
      std::cerr << node.line << ":" << node.pos << ": unknown binary operator"
                << std::endl;
      error_flag = true;
      return;
    }
    set_value_result( binary_instr );
  }
}

void SafeCIRBuilder::visit( unaryop_expr_node &node )
{
  if ( JJY_DEBUG_IR ) {
    printf( "%s %s unaryop_expr_node\n", JJY_DEBUG_SIGN, __func__ );
  }
  // DONE: handle unary operation.

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
      // op1 is not const
      // nothing to do
    }
  } else {
    op1_is_const = true;
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
  // DONE: handle lval.

  auto name        = node.name;
  VarInfo var_info = lookup_variable( name );
  if ( !var_info.is_valid( ) ) {
    std::cerr << node.line << ":" << node.pos << ": variable '" << name
              << "' is not declared" << std::endl;
    error_flag = true;
    return;
  }
  llvm::Type *int32_type  = llvm::Type::getInt32Ty( context );
  llvm::Value *zero_const = llvm::ConstantInt::get( int32_type, 0 );

  if ( !var_info.is_array ) {
    if ( JJY_DEBUG_IR ) {
      printf( "%s %s lval_node: not array\n", JJY_DEBUG_SIGN, __func__ );
    }

    if ( IS_EXPECT_LVAL( ) ) {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s lval_node: not array, expect lval\n", JJY_DEBUG_SIGN, __func__ );
      }
      set_value_result( var_info.val_ptr );
    } else {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s lval_node: not array, expect rval\n", JJY_DEBUG_SIGN, __func__ );
      }
      llvm::Value *loaded_val = builder.CreateLoad( int32_type, var_info.val_ptr, name );
      set_value_result( loaded_val );
    }

  } else {
    if ( JJY_DEBUG_IR ) {
      printf( "%s %s lval_node: is_array\n", JJY_DEBUG_SIGN, __func__ );
    }
    llvm::ArrayType *array_type =
        llvm::ArrayType::get( int32_type, var_info.array_length );
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
          // index is a variable

          // load the index_value and check if it is in [0, length)
          llvm::Value *index_load = builder.CreateLoad( int32_type, index_value, "" );
          llvm::Value *index_load_const =
              builder.CreateIntCast( index_load, int32_type, true );
          if ( var_info.is_obc ) {
            obc_check( index_load_const, var_info.array_length, node.line, node.pos,
                       name );
          }
          // get array element_ptr
          llvm::Value *element_ptr = builder.CreateGEP(
              array_type, var_info.val_ptr, { zero_const, index_load_const } );
          set_value_result( element_ptr );
        }
      } else {
        // index is a constant

        // get the index const
        llvm::Value *index_const = llvm::ConstantInt::get( int32_type, index_int );
        if ( var_info.is_obc ) {
          obc_check( index_const, var_info.array_length, node.line, node.pos, name );
        }
        // get array element_ptr
        llvm::Value *element_ptr = builder.CreateGEP( array_type, var_info.val_ptr,
                                                      { zero_const, index_const } );
        set_value_result( element_ptr );
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

          // load the index_value
          if ( var_info.is_obc ) {
            obc_check( index_value, var_info.array_length, node.line, node.pos, name );
          }
          llvm::Value *element_ptr = builder.CreateGEP( array_type, var_info.val_ptr,
                                                        { zero_const, index_value } );
          // load the element_ptr
          llvm::Value *element_value =
              builder.CreateLoad( int32_type, element_ptr, name );
          set_value_result( element_value );
        }
      } else {
        // index is a constant

        // get array element
        llvm::Value *index_value = llvm::ConstantInt::get( int32_type, index_int );
        if ( var_info.is_obc ) {
          obc_check( index_value, var_info.array_length, node.line, node.pos, name );
        }
        // get array element_ptr
        llvm::Value *element_ptr   = builder.CreateGEP( array_type, var_info.val_ptr,
                                                        { zero_const, index_value } );
        // load the element_ptr
        llvm::Value *element_value = builder.CreateLoad( int32_type, element_ptr, name );
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
  // DONE: handle variable definition.

  std::string name                   = node.name;
  bool is_const                      = node.is_const;
  bool is_obc                        = node.is_obc;
  ptr<expr_node> array_length        = node.array_length;
  ptr_vector<expr_node> initializers = node.initializers;
  llvm::Type *int32_type             = llvm::Type::getInt32Ty( context );
  llvm::Value *zero_const            = llvm::ConstantInt::get( int32_type, 0 );
  llvm::GlobalValue::LinkageTypes linkage =
      llvm::GlobalValue::LinkageTypes::ExternalLinkage;

  // check if the variable is declared more than one times
  VarInfo var_info = lookup_variable( name );
  size_t scope_index;
  for ( auto it = scoped_variables.rbegin( ); it != scoped_variables.rend( ); ++it ) {
    if ( it->variable_map.count( name ) ) {
      scope_index = std::distance( it, scoped_variables.rend( ) );
      break;
    }
  }
  if ( var_info.is_valid( ) && scoped_variables.size( ) == scope_index ) {
    std::cerr << node.line << ":" << node.pos << ": variable '" << name
              << "' is declared more than one times" << std::endl;
    error_flag = true;
    return;
  }

  if ( cur_scope == FLAG::GLOBAL_SCOPE ) {
    // global define
    llvm::GlobalVariable *global_variable;
    if ( !array_length ) {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s var_def_node: global not array\n", JJY_DEBUG_SIGN, __func__ );
      }
      // not an global array
      global_variable = new llvm::GlobalVariable( *module, int32_type, is_const, linkage,
                                                  nullptr, name );

      if ( initializers.size( ) > 0 ) {
        int init_int;
        EXPECT_LVAL( initializers[ 0 ]->accept( *this ) );
        if ( !get_int_result( init_int ) ) {
          std::cerr << node.line << ":" << node.pos << ": initializer must be a constant"
                    << std::endl;
          error_flag = true;
          return;
        }
        global_variable->setInitializer( llvm::ConstantInt::get( int32_type, init_int ) );
      } else {
        global_variable->setInitializer( llvm::ConstantInt::get( int32_type, 0 ) );
      }
      declare_variable( name, global_variable, is_const, false, is_obc, 0 );
    } else {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s var_def_node: global is array\n", JJY_DEBUG_SIGN, __func__ );
      }
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
                // init is not const
                // nothing to do
              }
            } else {
              // init is const
              global_array_init_values.push_back( builder.getInt32( init_int ) );
            }
          } else {
            // init is not provided
            global_array_init_values.push_back( builder.getInt32( 0 ) );
          }
        }
        llvm::ArrayType *array_type = llvm::ArrayType::get( int32_type, length );
        llvm::ArrayRef<llvm::Constant *> global_array_ref( global_array_init_values );
        llvm::Constant *global_array_init_const =
            llvm::ConstantArray::get( array_type, global_array_ref );

        global_variable = new llvm::GlobalVariable(
            *module, int32_type, is_const, linkage, global_array_init_const, name );
        declare_variable( name, global_variable, is_const, true, is_obc, length );
      }
    }
  } else {
    // local define
    llvm::AllocaInst *local_variable;

    if ( !array_length ) {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s var_def_node: local not array\n", JJY_DEBUG_SIGN, __func__ );
      }
      // not an local array
      local_variable = builder.CreateAlloca( int32_type, nullptr, name );
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
            // init is not const
            builder.CreateStore( init_value, local_variable );
          }
        } else {
          // init is const
          builder.CreateStore( builder.getInt32( init_int ), local_variable );
        }
      }
      declare_variable( name, local_variable, is_const, false, is_obc, 0 );
    } else {
      if ( JJY_DEBUG_IR ) {
        printf( "%s %s var_def_node: local is array\n", JJY_DEBUG_SIGN, __func__ );
      }
      int length;
      array_length->accept( *this );
      if ( !get_int_result( length ) ) {
        std::cerr << node.line << ":" << node.pos << ": array length must be a constant"
                  << std::endl;
        error_flag = true;
        return;
      }
      llvm::ArrayType *array_type = llvm::ArrayType::get( int32_type, length );
      local_variable              = builder.CreateAlloca( array_type, nullptr, name );
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
          llvm::Value *index = builder.getInt32( i );
          llvm::Value *element_ptr =
              builder.CreateGEP( array_type, local_variable, { zero_const, index } );
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
            builder.CreateStore( zero_const, element_ptr );
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
  // DONE: get target's rval and store at value's lval.

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
  // DONE: implement if-else statement.

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
  // DONE: implement while statement.

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
  // DONE: find the nearest decalred variable `name`

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
