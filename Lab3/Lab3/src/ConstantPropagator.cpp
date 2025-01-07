#include "ConstantPropagator.h"
#include "Utils.h"

#include <iostream>

char ConstValuePass::ID = 0;
int KSet::kset_size     = 16;

inline llvm::raw_ostream &operator<<( llvm::raw_ostream &out, const ConstValueMap &cv )
{
  if ( JJY_DEBUG_OPT ) {
    // llvm::outs( ) << JJY_DEBUG_SIGN << " " << __FILE_NAME__ << " " << __func__ << " "
    //               << __LINE__ << "\n";
  }
  // TODO: Implement if you want to print ConstValueMap with something like: llvm::outs()
  // << cv;
  out << "ConstValueMap {\n";
  for ( const auto &pair : cv ) {
    out << "  " << *pair.first << " -> "; // 打印 LLVM Value*
    if ( pair.second ) {
      out << pair.second->toString( ); // 打印 KSet 的内容
    } else {
      out << "null";
    }
    out << "\n";
  }
  out << "  }\n";
  return out;
}

// 打印 ConstValueState
inline llvm::raw_ostream &operator<<( llvm::raw_ostream &out,
                                      const ConstValueState &state )
{
  if ( JJY_DEBUG_OPT ) {
    // llvm::outs( ) << JJY_DEBUG_SIGN << " " << __FILE_NAME__ << " " << __func__ << " "
    //               << __LINE__ << "\n";
  }
  // TODO: Implement if you want to print ConstValueState with something like:
  // llvm::outs() << state;
  out << "ConstValueState {\n";
  out << "  ConstValueMap: " << state.cvmap; // 使用上面定义的运算符
  out << "}\n";
  return out;
}

ConstantPropagatorVisitor::ConstantPropagatorVisitor( ) {}

ConstantPropagatorVisitor::ConstantPropagatorVisitor(
    llvm::Module *module, int kset_size, DataflowBBResult<ConstValueState>::Type *res )
  : M( module ), result( res )
{
  // for all global variables
  KSet::kset_size = kset_size;
  for ( auto it = module->global_begin( ); it != module->global_end( ); it++ ) {
    global_variable.insert( &( *it ) );
  }
}

void ConstantPropagatorVisitor::compDFVal( llvm::Instruction *inst,
                                           ConstValueState *state )
{
  if ( JJY_DEBUG_OPT ) {
    llvm::outs( ) << JJY_DEBUG_SIGN << " " << __FILE_NAME__ << " " << __func__ << " "
                  << __LINE__;
    llvm::outs( ) << " inst: " << *inst << "\n";
    if ( JJY_DEBUG_OPT_OUTPUT ) {
      llvm::outs( ) << "state: " << *state << "\n";
    }
  }
  // TODO: hanldes different kinds of instructions
  //
  // 根据指令类型分别处理
  if ( llvm::BinaryOperator *binOp = llvm::dyn_cast<llvm::BinaryOperator>( inst ) ) {
    if ( JJY_DEBUG_OPT ) {
      llvm::outs( ) << JJY_DEBUG_SIGN << " BinaryOperator: " << *binOp << "\n";
    }
    // 处理二元运算指令（加减乘除等）
    llvm::Value *op1 = binOp->getOperand( 0 );
    llvm::Value *op2 = binOp->getOperand( 1 );

    // 获取操作数的常量集合
    KSet *kset1      = nullptr;
    KSet *kset2      = nullptr;

    if ( llvm::ConstantInt *constOp1 = llvm::dyn_cast<llvm::ConstantInt>( op1 ) ) {
      if ( JJY_DEBUG_OPT ) {
        llvm::outs( ) << JJY_DEBUG_SIGN << " constOp1\n";
      }
      kset1 = new KSet( );
      kset1->const_vals.insert( constOp1->getSExtValue( ) );
    } else if ( state->cvmap.find( op1 ) != state->cvmap.end( ) ) {
      if ( JJY_DEBUG_OPT ) {
        llvm::outs( ) << JJY_DEBUG_SIGN << " op1 in cvmap\n";
      }
      kset1 = state->cvmap[ op1 ];
    }

    if ( llvm::ConstantInt *constOp2 = llvm::dyn_cast<llvm::ConstantInt>( op2 ) ) {
      if ( JJY_DEBUG_OPT ) {
        llvm::outs( ) << JJY_DEBUG_SIGN << " constOp2\n";
      }
      kset2 = new KSet( );
      kset2->const_vals.insert( constOp2->getSExtValue( ) );
    } else if ( state->cvmap.find( op2 ) != state->cvmap.end( ) ) {
      if ( JJY_DEBUG_OPT ) {
        llvm::outs( ) << JJY_DEBUG_SIGN << " op2 in cvmap\n";
      }
      kset2 = state->cvmap[ op2 ];
    }

    // 创建新的KSet存储结果
    KSet *result = new KSet( );

    // 如果任一操作数不是常量，结果为TOP
    if ( !kset1 || !kset2 || kset1->top || kset2->top ) {
      if ( JJY_DEBUG_OPT ) {
        llvm::outs( ) << JJY_DEBUG_SIGN
                      << " !kset1 || !kset2 || kset1->top || kset2->top\n";
        // llvm::outs( ) << JJY_DEBUG_SIGN << " kset1->top = " << kset1->top
        //               << ", kset2->top = " << kset2->top << "\n";
      }
      result->top = true;
    } else {
      if ( JJY_DEBUG_OPT ) {
        llvm::outs( ) << JJY_DEBUG_SIGN
                      << " kset1->const_vals.size() = " << kset1->const_vals.size( )
                      << ", kset2->const_vals.size() = " << kset2->const_vals.size( )
                      << "\n";
      }
      // 对两个操作数的所有可能值进行运算
      for ( int val1 : kset1->const_vals ) {
        for ( int val2 : kset2->const_vals ) {
          int res;
          switch ( binOp->getOpcode( ) ) {
          case llvm::Instruction::Add:
            res = val1 + val2;
            break;
          case llvm::Instruction::Sub:
            res = val1 - val2;
            break;
          case llvm::Instruction::Mul:
            res = val1 * val2;
            break;
          case llvm::Instruction::SDiv:
            if ( val2 == 0 ) {
              result->top = true;
              goto end_compute;
            }
            res = val1 / val2;
            break;
          case llvm::Instruction::SRem:
            if ( val2 == 0 ) {
              result->top = true;
              goto end_compute;
            }
            res = val1 % val2;
            break;
          case llvm::Instruction::Or:
            if ( JJY_DEBUG_OPT ) {
              llvm::outs( ) << JJY_DEBUG_SIGN << " Or: " << val1 << " | " << val2 << "\n";
            }
            res = val1 | val2;
            break;
          default:
            result->top = true;
            goto end_compute;
          }
          result->const_vals.insert( res );
        }
      }
    }
  end_compute:
    state->cvmap[ inst ] = result;
  }

  // %15 = getelementptr [8 x i32], ptr %x, i32 0, i32 %idx4
  else if ( llvm::GetElementPtrInst *gepInst =
                llvm::dyn_cast<llvm::GetElementPtrInst>( inst ) ) {
    // 处理GEP指令
    // GEP指令的结果是一个指针，可以根据基地址和偏移量的常量值计算
    llvm::Value *ptr = gepInst->getPointerOperand( );
    llvm::Value *idx = gepInst->getOperand( 2 );

    KSet *ptr_kset   = nullptr;
    if ( llvm::ConstantInt *constPtr = llvm::dyn_cast<llvm::ConstantInt>( ptr ) ) {
      ptr_kset = new KSet( );
      ptr_kset->const_vals.insert( constPtr->getSExtValue( ) );
    } else if ( state->cvmap.find( ptr ) != state->cvmap.end( ) ) {
      ptr_kset = state->cvmap[ ptr ];
    }

    KSet *idx_kset = nullptr;
    if ( llvm::ConstantInt *constIdx = llvm::dyn_cast<llvm::ConstantInt>( idx ) ) {
      idx_kset = new KSet( );
      idx_kset->const_vals.insert( constIdx->getSExtValue( ) );
    } else if ( state->cvmap.find( idx ) != state->cvmap.end( ) ) {
      idx_kset = state->cvmap[ idx ];
    }

    // 如果指针或索引不是常量，结果设为TOP
    if ( ( !ptr_kset || ptr_kset->top ) && ( !idx_kset || idx_kset->top ) ) {
      if ( JJY_DEBUG_OPT ) {
        llvm::outs( ) << JJY_DEBUG_SIGN << " GEP: not const\n";
      }
      KSet *result         = new KSet( );
      result->top          = true;
      state->cvmap[ inst ] = result;
    } else {
      // 如果指针和索引都是常量，结果设为指针的常量值
      KSet *result         = new KSet( );
      result->top          = false;
      result->const_vals   = ( ptr_kset ) ? ptr_kset->const_vals : idx_kset->const_vals;
      state->cvmap[ inst ] = result;
    }
  }

  else if ( llvm::LoadInst *loadInst = llvm::dyn_cast<llvm::LoadInst>( inst ) ) {
    llvm::Value *ptr = loadInst->getPointerOperand( );

    // load
    // 创建新的KSet存储加载结果
    KSet *result     = new KSet( );

    // 如果指针在cvmap中有对应的KSet，使用该KSet的值
    if ( state->cvmap.find( ptr ) != state->cvmap.end( ) ) {
      if ( JJY_DEBUG_OPT ) {
        llvm::outs( ) << JJY_DEBUG_SIGN << " Load: from cvmap\n";
      }
      KSet *ptr_kset     = state->cvmap[ ptr ];
      result->top        = ptr_kset->top;
      result->const_vals = ptr_kset->const_vals;
    } else if ( global_variable.find( ptr ) != global_variable.end( ) ) {
      if ( JJY_DEBUG_OPT ) {
        llvm::outs( ) << JJY_DEBUG_SIGN << " Load: global\n";
      }
      // 如果是@input_var，则设置为top
      if ( ptr->getName( ) == "input_var" ) {
        result->top = true;
      } else {
        // 如果是全局变量，直接获取其常量值，
        // 获取找到的全局变量的常量值
        if ( global_state.cvmap.find( ptr ) != global_state.cvmap.end( ) ) {
          result->top        = global_state.cvmap[ ptr ]->top;
          result->const_vals = global_state.cvmap[ ptr ]->const_vals;
        } else {
          llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>( ptr );
          llvm::ConstantInt *constPtr =
              llvm::dyn_cast<llvm::ConstantInt>( globalVar->getInitializer( ) );
          result->const_vals.insert( constPtr->getSExtValue( ) );
        }
      }
    } else {
      // 如果在cvmap中找不到对应的KSet，结果设为TOP
      if ( JJY_DEBUG_OPT ) {
        llvm::outs( ) << JJY_DEBUG_SIGN << " Load: not found, set to TOP\n";
      }
      result->top = true;
    }

    state->cvmap[ inst ] = result;
    // // 处理加载指令
    // // 加载指令的结果是存储在内存中的值，可以根据内存地址的常量值计算
    // llvm::Value *ptr = loadInst->getPointerOperand( );

    // KSet *result     = new KSet( );

    // result->top      = false;

    // // 如果指针是全局变量，可以直接获取其常量值

    // if ( global_variable.find( ptr ) != global_variable.end( ) ) {
    //   if ( JJY_DEBUG_OPT ) {
    //     llvm::outs( ) << JJY_DEBUG_SIGN << " Load: global\n";
    //   }
    //   // 如果是全局变量，直接获取其常量值，

    //   // 获取找到的全局变量的常量值

    //   llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>( ptr );
    //   llvm::ConstantInt *constPtr =
    //       llvm::dyn_cast<llvm::ConstantInt>( globalVar->getInitializer( ) );
    //   result->const_vals.insert( constPtr->getSExtValue( ) );

    // } else if ( llvm::ConstantInt *constPtr = llvm::dyn_cast<llvm::ConstantInt>( ptr )
    // ) {
    //   if ( JJY_DEBUG_OPT ) {
    //     llvm::outs( ) << JJY_DEBUG_SIGN << " Load: const\n";
    //   }
    //   // 如果是常量指针，直接获取其常量值
    //   result->const_vals.insert( constPtr->getSExtValue( ) );
    // } else if ( state->cvmap.find( ptr ) != state->cvmap.end( ) ) {
    //   if ( JJY_DEBUG_OPT ) {
    //     llvm::outs( ) << JJY_DEBUG_SIGN << " Load: var\n";
    //   }
    //   // 如果是变量，获取其常量值
    //   result->const_vals = state->cvmap[ ptr ]->const_vals;
    // } else {
    //   // 其他情况，结果设为TOP
    //   if ( JJY_DEBUG_OPT ) {
    //     llvm::outs( ) << JJY_DEBUG_SIGN << " Load: other case\n";
    //   }
    //   result->top = true;
    // }

    // state->cvmap[ inst ] = result;

  } else if ( llvm::StoreInst *storeInst = llvm::dyn_cast<llvm::StoreInst>( inst ) ) {
    llvm::Value *val = storeInst->getValueOperand( );
    llvm::Value *ptr = storeInst->getPointerOperand( );

    // 判断ptr是否为全局变量指针
    // if ( global_variable.find( ptr ) != global_variable.end( ) ) {
    //   if ( JJY_DEBUG_OPT ) {
    //     llvm::outs( ) << JJY_DEBUG_SIGN << " Store: global\n";
    //   }
    // }

    // 获取或创建pointer的KSet
    KSet *ptr_kset   = nullptr;
    if ( state->cvmap.find( ptr ) == state->cvmap.end( ) ) {
      ptr_kset            = new KSet( );
      state->cvmap[ ptr ] = ptr_kset;
    } else {
      ptr_kset = state->cvmap[ ptr ];
    }

    // 更新pointer的KSet
    if ( llvm::ConstantInt *constVal = llvm::dyn_cast<llvm::ConstantInt>( val ) ) {
      // value是常量
      ptr_kset->top = false;
      ptr_kset->const_vals.clear( );
      ptr_kset->const_vals.insert( constVal->getSExtValue( ) );
      if ( global_variable.find( ptr ) != global_variable.end( ) &&
           !is_func_call_processing ) {
        // auto old_global_state     = global_state;
        // global_state.cvmap[ ptr ] = ptr_kset;

        if ( JJY_DEBUG_OPT_FUNC ) {
          llvm::outs( ) << JJY_DEBUG_SIGN << " Store Global Const\n";
        }
        if ( global_state.cvmap.find( ptr ) == global_state.cvmap.end( ) ) {
          global_state.cvmap[ ptr ] = new KSet( );
        }
        global_state.cvmap[ ptr ]->top = ptr_kset->top;
        // global_state.cvmap[ ptr ]->const_vals = ptr_kset->const_vals;
        global_state.cvmap[ ptr ]->const_vals.insert( ptr_kset->const_vals.begin( ),
                                                      ptr_kset->const_vals.end( ) );

        // if ( old_global_state != global_state ) {
        //   is_global_state_changed = true;
        //   if ( JJY_DEBUG_OPT ) {
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " Store: global\n";
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " old global_state: " <<
        //     old_global_state
        //                   << "\n";
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " global_state: " << global_state <<
        //     "\n";
        //   }
        //   if ( JJY_DEBUG_OPT_FUNC ) {
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " Store: global\n";
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " old global_state: " <<
        //     old_global_state
        //                   << "\n";
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " global_state: " << global_state <<
        //     "\n";
        //   }
        // }
      }
    } else if ( state->cvmap.find( val ) != state->cvmap.end( ) ) {
      // value有对应的KSet
      KSet *val_kset       = state->cvmap[ val ];
      ptr_kset->top        = val_kset->top;
      ptr_kset->const_vals = val_kset->const_vals;
      if ( global_variable.find( ptr ) != global_variable.end( ) &&
           !is_func_call_processing ) {
        // auto old_global_state     = global_state;

        if ( JJY_DEBUG_OPT_FUNC ) {
          llvm::outs( ) << JJY_DEBUG_SIGN << " Store Global Var\n";
        }
        if ( global_state.cvmap.find( ptr ) == global_state.cvmap.end( ) ) {
          global_state.cvmap[ ptr ] = new KSet( );
        }
        global_state.cvmap[ ptr ]->top = val_kset->top;
        global_state.cvmap[ ptr ]->const_vals.insert( val_kset->const_vals.begin( ),
                                                      val_kset->const_vals.end( ) );

        //
        //
        //
        // global_state.cvmap[ ptr ] = ptr_kset;
        // if ( old_global_state != global_state ) {
        //   is_global_state_changed = true;
        //   if ( JJY_DEBUG_OPT ) {
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " Store: global\n";
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " old global_state: " <<
        //     old_global_state
        //                   << "\n";
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " global_state: " << global_state <<
        //     "\n";
        //   }
        //   if ( JJY_DEBUG_OPT_FUNC ) {
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " Store: global\n";
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " old global_state: " <<
        //     old_global_state
        //                   << "\n";
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " global_state: " << global_state <<
        //     "\n";
        //   }
        // }
      }
    } else {
      // 其他情况设为top
      ptr_kset->top = true;
      ptr_kset->const_vals.clear( );
      if ( global_variable.find( ptr ) != global_variable.end( ) &&
           !is_func_call_processing ) {
        // auto old_global_state     = global_state;
        // global_state.cvmap[ ptr ] = ptr_kset;
        //

        if ( JJY_DEBUG_OPT_FUNC ) {
          llvm::outs( ) << JJY_DEBUG_SIGN << " Store Global Other\n";
        }
        if ( global_state.cvmap.find( ptr ) == global_state.cvmap.end( ) ) {
          global_state.cvmap[ ptr ] = new KSet( );
        }
        global_state.cvmap[ ptr ]->top = true;

        // if ( old_global_state != global_state ) {
        //   is_global_state_changed = true;
        //   if ( JJY_DEBUG_OPT ) {
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " Store: global\n";
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " old global_state: " <<
        //     old_global_state
        //                   << "\n";
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " global_state: " << global_state <<
        //     "\n";
        //   }
        //   if ( JJY_DEBUG_OPT_FUNC ) {
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " Store: global\n";
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " old global_state: " <<
        //     old_global_state
        //                   << "\n";
        //     llvm::outs( ) << JJY_DEBUG_SIGN << " global_state: " << global_state <<
        //     "\n";
        //   }
        // }
      }
    }
    // llvm::Value *val = storeInst->getValueOperand( );
    // llvm::Value *ptr = storeInst->getPointerOperand( );

    // // 全局变量
    // KSet *ptr_kset   = nullptr;
    // if ( global_variable.find( ptr ) != global_variable.end( ) ) {
    //   if ( JJY_DEBUG_OPT ) {
    //     llvm::outs( ) << JJY_DEBUG_SIGN << " Store: global\n";
    //   }
    //   // 如果是全局变量，直接获取其常量值，
    //   // 获取找到的全局变量的常量值
    //   llvm::GlobalVariable *globalVar = llvm::dyn_cast<llvm::GlobalVariable>( ptr );
    //   llvm::ConstantInt *constPtr =
    //       llvm::dyn_cast<llvm::ConstantInt>( globalVar->getInitializer( ) );
    //   ptr_kset = new KSet( );
    //   ptr_kset->const_vals.insert( constPtr->getSExtValue( ) );
    //   state->cvmap[ ptr ] = ptr_kset;
    // }
    // // 创建或获取pointer指向变量的KSet
    // else if ( state->cvmap.find( ptr ) == state->cvmap.end( ) ) {
    //   ptr_kset            = new KSet( );
    //   state->cvmap[ ptr ] = ptr_kset;
    // } else {
    //   ptr_kset = state->cvmap[ ptr ];
    // }

    // // 根据stored value更新pointer的KSet
    // if ( llvm::ConstantInt *constVal = llvm::dyn_cast<llvm::ConstantInt>( val ) ) {
    //   ptr_kset->top = false;
    //   ptr_kset->const_vals.clear( );
    //   ptr_kset->const_vals.insert( constVal->getSExtValue( ) );
    // } else if ( state->cvmap.find( val ) != state->cvmap.end( ) ) {
    //   KSet *val_kset       = state->cvmap[ val ];
    //   ptr_kset->top        = val_kset->top;
    //   ptr_kset->const_vals = val_kset->const_vals;
    // } else {
    //   ptr_kset->top = true;
    //   ptr_kset->const_vals.clear( );
    // }
    // // 处理存储指令
    // // TODO: 现在store 变量 还全是 TOP
    // //
    // // 存储指令的结果是存储在内存中的值，可以根据内存地址的常量值计算
    // llvm::Value *val = storeInst->getValueOperand( );

    // KSet *val_kset   = nullptr;
    // if ( llvm::ConstantInt *constVal = llvm::dyn_cast<llvm::ConstantInt>( val ) ) {
    //   if ( JJY_DEBUG_OPT ) {
    //     llvm::outs( ) << JJY_DEBUG_SIGN << " Store: val_const\n";
    //   }
    //   val_kset = new KSet( );
    //   val_kset->const_vals.insert( constVal->getSExtValue( ) );
    // } else if ( state->cvmap.find( val ) != state->cvmap.end( ) ) {
    //   if ( JJY_DEBUG_OPT ) {
    //     llvm::outs( ) << JJY_DEBUG_SIGN << " Store: val_var\n";
    //   }
    //   val_kset = state->cvmap[ val ];
    // }

    // // 获取存储指针的常量值
    // llvm::Value *ptr = storeInst->getPointerOperand( );
    // KSet *ptr_kset   = nullptr;
    // if ( llvm::ConstantInt *constPtr = llvm::dyn_cast<llvm::ConstantInt>( ptr ) ) {
    //   if ( JJY_DEBUG_OPT ) {
    //     llvm::outs( ) << JJY_DEBUG_SIGN << " Store: ptr_const\n";
    //   }
    //   ptr_kset = new KSet( );
    //   ptr_kset->const_vals.insert( constPtr->getSExtValue( ) );
    // } else if ( state->cvmap.find( ptr ) != state->cvmap.end( ) ) {
    //   if ( JJY_DEBUG_OPT ) {
    //     llvm::outs( ) << JJY_DEBUG_SIGN << " Store: ptr_var\n";
    //   }
    //   ptr_kset = state->cvmap[ ptr ];
    // }

    // // 如果指针或存储值不是常量，结果设为TOP
    // if ( ( !val_kset || val_kset->top ) && ( !ptr_kset || ptr_kset->top ) ) {
    //   if ( JJY_DEBUG_OPT ) {
    //     llvm::outs( ) << JJY_DEBUG_SIGN << " Store: not const\n";
    //   }
    //   KSet *result         = new KSet( );
    //   result->top          = true;
    //   state->cvmap[ inst ] = result;
    // } else {
    //   // 如果指针和存储值都是常量，结果设为存储值
    //   KSet *result         = new KSet( );
    //   result->top          = false;
    //   // result->const_vals   = val_kset->const_vals;
    //   result->const_vals   = ( val_kset ) ? val_kset->const_vals :
    //   ptr_kset->const_vals; state->cvmap[ inst ] = result;
    // }
  } else if ( llvm::ICmpInst *icmpInst = llvm::dyn_cast<llvm::ICmpInst>( inst ) ) {
    // 处理比较指令
    // 比较指令的结果是布尔值，可以根据操作数的常量值计算
    llvm::Value *op1 = icmpInst->getOperand( 0 );
    llvm::Value *op2 = icmpInst->getOperand( 1 );

    KSet *result     = new KSet( );

    result->top      = false;

    // 获取操作数的常量集合
    KSet *kset1      = nullptr;
    KSet *kset2      = nullptr;

    if ( llvm::ConstantInt *constOp1 = llvm::dyn_cast<llvm::ConstantInt>( op1 ) ) {
      kset1 = new KSet( );
      kset1->const_vals.insert( constOp1->getSExtValue( ) );
    } else if ( state->cvmap.find( op1 ) != state->cvmap.end( ) ) {
      kset1 = state->cvmap[ op1 ];
    }

    if ( llvm::ConstantInt *constOp2 = llvm::dyn_cast<llvm::ConstantInt>( op2 ) ) {
      kset2 = new KSet( );
      kset2->const_vals.insert( constOp2->getSExtValue( ) );
    } else if ( state->cvmap.find( op2 ) != state->cvmap.end( ) ) {
      kset2 = state->cvmap[ op2 ];
    }

    // 如果任一操作数不是常量，结果为TOP

    if ( !kset1 || !kset2 || kset1->top || kset2->top ) {
      result->top = true;
    } else {
      // 对两个操作数的所有可能值进行比较
      for ( int val1 : kset1->const_vals ) {
        for ( int val2 : kset2->const_vals ) {
          bool res;
          switch ( icmpInst->getPredicate( ) ) {
          case llvm::CmpInst::ICMP_EQ:
            res = val1 == val2;
            if ( JJY_DEBUG_OPT ) {
              llvm::outs( ) << JJY_DEBUG_SIGN << " icmp: " << *icmpInst << "\n";
              llvm::outs( ) << JJY_DEBUG_SIGN << " val1: " << val1 << " val2: " << val2
                            << " res: " << res << "\n";
            }
            break;
          case llvm::CmpInst::ICMP_NE:
            res = val1 != val2;
            if ( JJY_DEBUG_OPT ) {
              llvm::outs( ) << JJY_DEBUG_SIGN << " icmp: " << *icmpInst << "\n";
              llvm::outs( ) << JJY_DEBUG_SIGN << " val1: " << val1 << " val2: " << val2
                            << " res: " << res << "\n";
            }
            break;
          case llvm::CmpInst::ICMP_SGT:
            res = val1 > val2;
            if ( JJY_DEBUG_OPT ) {
              llvm::outs( ) << JJY_DEBUG_SIGN << " icmp: " << *icmpInst << "\n";
              llvm::outs( ) << JJY_DEBUG_SIGN << " val1: " << val1 << " val2: " << val2
                            << " res: " << res << "\n";
            }
            break;
          case llvm::CmpInst::ICMP_SGE:
            res = val1 >= val2;
            if ( JJY_DEBUG_OPT ) {
              llvm::outs( ) << JJY_DEBUG_SIGN << " icmp: " << *icmpInst << "\n";
              llvm::outs( ) << JJY_DEBUG_SIGN << " val1: " << val1 << " val2: " << val2
                            << " res: " << res << "\n";
            }
            break;
          case llvm::CmpInst::ICMP_SLT:
            res = val1 < val2;
            if ( JJY_DEBUG_OPT ) {
              llvm::outs( ) << JJY_DEBUG_SIGN << " icmp: " << *icmpInst << "\n";
              llvm::outs( ) << JJY_DEBUG_SIGN << " val1: " << val1 << " val2: " << val2
                            << " res: " << res << "\n";
            }
            break;
          case llvm::CmpInst::ICMP_SLE:
            res = val1 <= val2;
            if ( JJY_DEBUG_OPT ) {
              llvm::outs( ) << JJY_DEBUG_SIGN << " icmp: " << *icmpInst << "\n";
              llvm::outs( ) << JJY_DEBUG_SIGN << " val1: " << val1 << " val2: " << val2
                            << " res: " << res << "\n";
            }
            break;
          default:
            if ( JJY_DEBUG_OPT ) {
              llvm::outs( ) << JJY_DEBUG_SIGN << " Other icmp: " << *icmpInst << "\n";
            }
            result->top = true;
          }
          if ( res ) {
            result->const_vals.insert( 1 );
          } else {
            result->const_vals.insert( 0 );
          }
        }
      }
    }
    // add result to state
    state->cvmap[ inst ] = result;
  }
  // call指令
  else if ( llvm::CallInst *callInst = llvm::dyn_cast<llvm::CallInst>( inst ) ) {
    // is_call_inst    = true;
    // function_called = callInst->getCalledFunction( );
    auto func                   = callInst->getCalledFunction( );
    const std::string func_name = func->getName( ).str( );
    if ( func_name == "obc_check_error" || func_name == "input" ||
         func_name == "output" || func_name == "input_impl" ||
         func_name == "output_impl" ) {
      if ( JJY_DEBUG_OPT ) {
        llvm::outs( ) << JJY_DEBUG_SIGN << " Call to " << func_name << ", skip" << "\n";
      }
      return;
    } else {
      func_state[ func ]      = global_state;
      // is_call_inst    = true;
      // function_called = callInst->getCalledFunction( );
      // llvm::outs( ) << JJY_DEBUG_SIGN << " state: " << *state << "\n";
      // llvm::outs( ) << JJY_DEBUG_SIGN << " trying to recalc: " <<
      // (function_called)->getName() << "\n";

      // 遍历函数的所有基本块的所有指令

      is_func_call_processing = true;
      // std::map<llvm::BasicBlock *, std::pair<ConstValueState *, ConstValueState *>>
      //     bb_state;
      // std::vector<llvm::BasicBlock *> bb_list;
      // for ( llvm::BasicBlock &bb : *func ) {
      //   bb_list.push_back( &bb );
      //   bb_state[ &bb ] = std::make_pair( new ConstValueState( *state ),
      //                                     new ConstValueState( *state ) );
      // }
      // while ( !bb_list.empty( ) ) {
      //   llvm::BasicBlock &bb = *bb_list.back( );
      //   bb_list.pop_back( );
      //   // 判断bb是否为entry
      //   ConstValueState *in_state;
      //   ConstValueState *out_state;
      //   in_state = new ConstValueState( *state );
      //   if ( &bb == &( func->getEntryBlock( ) ) ) {
      //     //
      //   } else {
      //     for ( auto pred = pred_begin( &bb ); pred != pred_end( &bb ); ++pred ) {
      //       merge( in_state, bb_state[ *pred ].second );
      //     }
      //   }
      //   bb_state[ &bb ].first      = in_state;
      //   out_state                  = in_state;

      //   ConstValueState *state_tmp = out_state;
      //   if ( bb.getName( ).startswith( "obc_err_" ) ) {

      //   } else {

      //     // for ( llvm::Instruction &inst : bb ) {
      //     //   compDFVal( &inst, state_tmp );
      //     // }
      //     for ( llvm::BasicBlock::iterator ii = bb.begin( ), ie = bb.end( ); ii !=
      //     ie;
      //           ++ii ) {
      //       llvm::Instruction *inst = &*ii;
      //       compDFVal( inst, state_tmp );
      //     }
      //     // if ( block->getName( ) == "entry" && block->getParent( )->getName( ) ==
      //     // "main" ) {
      //     //   initGlobal( state );
      //     // }
      //     // for ( llvm::BasicBlock::iterator ii = block->begin( ), ie = block->end(
      //     );
      //     // ii != ie;
      //     //       ++ii ) {
      //     //   llvm::Instruction *inst = &*ii;
      //     //   compDFVal( inst, state );
      //     // }
      //   }
      //   out_state = state_tmp;
      //   if ( !( out_state == bb_state[ &bb ].second ) ) {
      //     bb_state[ &bb ].second = out_state;
      //     for ( auto succ = succ_begin( &bb ); succ != succ_end( &bb ); ++succ ) {
      //       bb_list.push_back( *succ );
      //     }
      //   }

      //   // bb_state[ &bb ].second = out_state;

      //   // for ( llvm::Instruction &inst : bb ) {
      //   //   // llvm::outs( ) << JJY_DEBUG_SIGN << " " << inst << "\n";
      //   //   compDFVal( &inst, state );
      //   // }
      //   // merge( state, old_state );

      //   // if ( BB == &fn->getEntryBlock( ) ) {
      //   //   in = initval;
      //   // } else {
      //   //   for ( auto pred = pred_begin( BB ); pred != pred_end( BB ); ++pred ) {
      //   //     visitor->merge( &in, &( *result )[ *pred ].second );
      //   //   }
      //   // }
      // }
      // state                   = bb_state[ &( func->back( ) ) ].second;
      for ( llvm::BasicBlock &bb : *func ) {
        for ( llvm::Instruction &inst : bb ) {
          compDFVal( &inst, state );
        }
      }
      // XXXX: 核心问题在于，函数调用过程的处理方式！
      // 这里理论上把合流算法写好就能跑了应该，但是1.5日清晨精力有限没写完，后面争取能够写完吧...
      // 目前13/16
      //
      is_func_call_processing = false;

      // 处理这个函数调用过程，并将这个函数调用的结果合流到state中
      // // 打印global_var state
      // for ( auto *gv : global_variable ) {
      //   if ( state->cvmap.find( gv ) != state->cvmap.end( ) ) {
      //     // 将gv的状态添加到被调用函数的参数中
      //     llvm::Function *F       = callInst->getCalledFunction( );
      //     // 获取F的entry block
      //     llvm::BasicBlock &entry = F->getEntryBlock( );

      //     // 打印 entry
      //     if ( JJY_DEBUG_OPT ) {
      //       llvm::outs( ) << JJY_DEBUG_SIGN << " entry: " << entry << "\n";
      //     }
      //   }
      // }
    }
  }

  // else if ( llvm::BranchInst *brInst = llvm::dyn_cast<llvm::BranchInst>( inst ) ) {
  //   if ( JJY_DEBUG_OPT ) {
  //     llvm::outs( ) << JJY_DEBUG_SIGN << " BranchInst: " << *brInst << "\n";
  //   }
  //   // 获取条件变量（可能为无条件），判断条件变量是否为TOP，且br是否为while循环(br
  //   // body_x)的判断条件
  //   //
  //   if ( brInst->isConditional( ) ) {
  //     llvm::Value *cond = brInst->getCondition( );
  //     if ( state->cvmap.find( cond ) != state->cvmap.end( ) ) {
  //       KSet *cond_kset = state->cvmap[ cond ];
  //       if ( cond_kset->top ) {
  //         // 如果条件变量为TOP，且br为while循环的判断条件
  //         // 则将while循环的body块的所有变量的常量传播为TOP
  //         auto br_stmt_1 = brInst->getSuccessor( 0 );
  //         if ( br_stmt_1->getName( ).startswith( "body" ) ) {
  //           if ( JJY_DEBUG_OPT ) {
  //             llvm::outs( ) << JJY_DEBUG_SIGN << " br_while" << "\n";
  //           }
  //           for ( llvm::Instruction &inst : *br_stmt_1 ) {
  //             if ( state->cvmap.find( &inst ) != state->cvmap.end( ) ) {
  //               state->cvmap[ &inst ]->top = true;
  //               state->cvmap[ &inst ]->const_vals.clear( );
  //             } else {
  //               KSet *result          = new KSet( );
  //               result->top           = true;
  //               state->cvmap[ &inst ] = result;
  //               if ( JJY_DEBUG_OPT ) {
  //                 llvm::outs( ) << JJY_DEBUG_SIGN << " br_while: " << inst
  //                               << " has set to top" << "\n";
  //               }
  //             }
  //           }
  //         }
  //       }
  //     }
  //   }
  //
  //
  //
  //
  //
  //
  //
  //
  //
  //
  //
  //
  //
  //
  //
  //
  // llvm::Value *cond = brInst->getCondition( );
  // if ( state->cvmap.find( cond ) != state->cvmap.end( ) ) {
  //   KSet *cond_kset = state->cvmap[ cond ];
  //   if ( cond_kset->top ) {
  //     // 如果条件变量为TOP，且br为while循环的判断条件
  //     // 则将while循环的body块的所有指令常量传播为TOP
  //     auto br_stmt_1 = brInst->getSuccessor( 0 );
  //     if ( br_stmt_1->getName( ).startswith( "body" ) ) {
  //       for ( llvm::Instruction &inst : *br_stmt_1 ) {
  //         if ( state->cvmap.find( &inst ) != state->cvmap.end( ) ) {
  //           state->cvmap[ &inst ]->top = true;
  //           state->cvmap[ &inst ]->const_vals.clear( );
  //         }
  //       }
  //     }
  //   }
  // }
  // }
  else {
    if ( JJY_DEBUG_OPT ) {
      llvm::outs( ) << JJY_DEBUG_SIGN << " Other inst: " << *inst << "\n";
    }

    // 其他指令类型，结果设为TOP
    // KSet *result         = new KSet( );
    // result->top          = true;
    // state->cvmap[ inst ] = result;
  }
}

void ConstantPropagatorVisitor::merge( ConstValueState *dest, ConstValueState *src )
{
  if ( JJY_DEBUG_OPT ) {
    llvm::outs( ) << JJY_DEBUG_SIGN << " " << __FILE_NAME__ << " " << __func__ << " "
                  << __LINE__ << "\n";
    // llvm::outs( ) << JJY_DEBUG_SIGN << " dest: " << *dest << "\n";
    // llvm::outs( ) << JJY_DEBUG_SIGN << " src: " << *src << "\n";
  }
  // TODO
  // 遍历源状态中的所有映射
  for ( auto &src_pair : src->cvmap ) {
    llvm::Value *val = src_pair.first;
    KSet *src_kset   = src_pair.second;

    // 检查目标状态中是否已有该值的映射
    if ( dest->cvmap.find( val ) == dest->cvmap.end( ) ) {
      // 如果目标中没有，直接复制源中的KSet
      KSet *new_kset = new KSet( );
      if ( src_kset->top ) {
        new_kset->top = true;
      } else {
        new_kset->const_vals = src_kset->const_vals;
      }
      dest->cvmap[ val ] = new_kset;
    } else {
      // 如果目标中已有，需要合并两个KSet
      KSet *dest_kset = dest->cvmap[ val ];

      // 如果任一为TOP，结果为TOP
      if ( dest_kset->top || src_kset->top ) {
        dest_kset->top = true;
        dest_kset->const_vals.clear( );
      } else {
        // 合并两个常量集合
        dest_kset->const_vals.insert( src_kset->const_vals.begin( ),
                                      src_kset->const_vals.end( ) );
      }
    }
  }
}

void ConstantPropagatorVisitor::initGlobal( ConstValueState *state )
{
  if ( JJY_DEBUG_OPT ) {
    llvm::outs( ) << JJY_DEBUG_SIGN << " " << __FILE_NAME__ << " " << __func__ << " "
                  << __LINE__ << "\n";
    if ( JJY_DEBUG_OPT_OUTPUT ) {
      llvm::outs( ) << "State: " << *state << "\n";
    }
  }
  // TODO
  // 清空现有的映射
  state->cvmap.clear( );

  // 如果需要初始化特定的全局变量或参数，可以在这里添加
  // 例如，对于main函数的参数 argc，我们知道它一定是非负的
  /*
  if (llvm::Function *main = module->getFunction("main")) {
      for (auto &arg : main->args()) {
          KSet *kset = new KSet();
          // 为特定参数设置初始值
          if (arg.getName() == "argc") {
              kset->const_vals.insert(1);  // 假设至少有一个参数
          } else {
              kset->top = true;  // 其他参数设为 TOP
          }
          state->cvmap[&arg] = kset;
      }
  }
  */
}

ConstValuePass::ConstValuePass( std::vector<llvm::BasicBlock *> obc_check_record,
                                int ksize )
  : ModulePass( ID )
{
  check_redundant = obc_check_record;
  kset_size       = ksize;
}

bool ConstValuePass::runOnModule( llvm::Module &M )
{
  if ( JJY_DEBUG_OPT ) {
    llvm::outs( ) << JJY_DEBUG_SIGN << " " << __FILE_NAME__ << " " << __func__ << " "
                  << __LINE__ << "\n";
  }
  ConstantPropagatorVisitor visitor( &M, kset_size, &result );

  std::set<llvm::Function *> f_worklist;
  for ( auto &F : M ) {
    if ( F.isIntrinsic( ) )
      continue;
    if ( F.getName( ) == "obc_check_error" || F.getName( ) == "input" ||
         F.getName( ) == "output" || F.getName( ) == "input_impl" ||
         F.getName( ) == "output_impl" )
      continue;
    f_worklist.insert( &F );
  }

  // TODO: Compute dataflow information for each function in f_worklist.
  // 对每个函数进行数据流分析
  auto f_worklist_copy = f_worklist;
  while ( !f_worklist.empty( ) ) {
    llvm::Function *F = *f_worklist.begin( );
    f_worklist.erase( F );

    if ( JJY_DEBUG_OPT ) {
      // print function name
      llvm::outs( ) << "============\n";
      llvm::outs( ) << JJY_DEBUG_SIGN << " Function: " << F->getName( ) << "\n";
    }
    // 创建初始状态
    ConstValueState initval;
    visitor.initGlobal( &initval );

    // 调用前向数据流分析框架
    typename DataflowBBResult<ConstValueState>::Type result_map;
    auto old_global_state = visitor.global_state;
    compForwardDataflow( F, &visitor, &result_map, initval );
    // if ( visitor.is_call_inst ) {
    //   visitor.is_call_inst = false;
    //   // 将visitor.function_called加入f_worklist
    //   f_worklist.insert( visitor.function_called );
    //   // continue;
    // }
    // if ( old_global_state != visitor.global_state ) {
    //   // 将f_worklist重置为f_worklist_copy
    //   if ( JJY_DEBUG_OPT_FUNC ) {
    //     llvm::outs( ) << JJY_DEBUG_SIGN << " Function: " << F->getName( )
    //                   << " changed the state\n";
    //     llvm::outs( ) << JJY_DEBUG_SIGN << " global_state: " << visitor.global_state
    //                   << "\n";
    //   }
    //   f_worklist = f_worklist_copy;
    //   continue;
    // }
    if ( !visitor.func_state.empty( ) ) {
      if ( !visitor.processing_call ) {
        visitor.global_state_bak = visitor.global_state;
      }
      visitor.processing_call = true;
      // 取出其中的第一个元素
      auto func_state         = visitor.func_state.begin( );
      // 将func_state的key放到f_worklist的开头
      f_worklist.insert( func_state->first );
      // 将func_state的value赋值给global_state
      visitor.global_state = func_state->second;
      // 将取出的第一个元素从func_state中删除
      visitor.func_state.erase( func_state );
      if ( JJY_DEBUG_OPT ) {
        llvm::outs( ) << JJY_DEBUG_SIGN << " Function: " << F->getName( )
                      << " changed the state\n";
        llvm::outs( ) << JJY_DEBUG_SIGN << " global_state: " << visitor.global_state
                      << "\n";
      }
    } else {
      visitor.processing_call = false;
      visitor.global_state    = visitor.global_state_bak;
    }

    // 将分析结果存储到全局结果中
    for ( auto &BB : *F ) {
      result[ &BB ] = result_map[ &BB ];
    }

    // check_redundant写入
    if ( 1 ) {
      for ( auto &BB : *F ) {
        // 检查BB.getName()是否以obc_check开头
        if ( BB.getName( ).startswith( "obc_check" ) ) {
          // std::vector<llvm::BasicBlock *> check_redundant;
          if ( JJY_DEBUG_OPT ) {
            llvm::outs( ) << "check_redundant: " << BB.getName( ) << "\n";
          }
          // 检查BB是否在check_redundant中，如不在则加入
          //
          if ( std::find( check_redundant.begin( ), check_redundant.end( ), &BB ) ==
               check_redundant.end( ) ) {
            check_redundant.push_back( &BB );
          }
          // check_redundant.push_back( &BB );
        }
      }
    }
  }

  // 打印check_redundant
  if ( JJY_DEBUG_OPT ) {
    llvm::outs( ) << "check_redundant: \n";
    for ( auto bb : check_redundant ) {
      llvm::outs( ) << *bb << "\n";
    }
  }

  removeRedundant( );

  return true;
}

void ConstValuePass::removeRedundant( )
{
  if ( JJY_DEBUG_OPT ) {
    llvm::outs( ) << JJY_DEBUG_SIGN << " " << __FILE_NAME__ << " " << __func__ << " "
                  << __LINE__ << "\n";
  }
  // TODO: You may need to adjust this to fit your implementation.
  llvm::outs( ) << "============== Remove Start ==============\n";
  // 遍历check_redundant 中的每一个基本块
  for ( auto check_bb : check_redundant ) {
    auto br_ins = check_bb->getTerminator( );
    auto err_bb = br_ins->getSuccessor( 0 );
    auto ok_bb  = br_ins->getSuccessor( 1 );

    // 获取两个比较指令
    llvm::Instruction *cond_val =
        llvm::dyn_cast<llvm::Instruction>( br_ins->getOperand( 0 ) );
    llvm::Instruction *upper_cmp =
        llvm::dyn_cast<llvm::Instruction>( cond_val->getOperand( 0 ) );
    llvm::Instruction *lower_cmp =
        llvm::dyn_cast<llvm::Instruction>( cond_val->getOperand( 1 ) );

    // 获取数组index，上界和下界
    llvm::Value *idx_val      = upper_cmp->getOperand( 0 );
    llvm::Value *uppper_bound = upper_cmp->getOperand( 1 );
    llvm::Value *lower_bound  = lower_cmp->getOperand( 1 );

    // idx belongs to [lower, upper)
    // 判断上界和下界是否是常数
    if ( llvm::isa<llvm::ConstantInt>( uppper_bound ) &&
         llvm::isa<llvm::ConstantInt>( lower_bound ) ) {
      // 获取上界和下界的int值
      int upper = llvm::dyn_cast<llvm::ConstantInt>( uppper_bound )->getSExtValue( );
      int lower = llvm::dyn_cast<llvm::ConstantInt>( lower_bound )->getSExtValue( );
      // llvm::outs( ) << "idx: " << *idx_val << " in [" << lower << ", " << upper <<
      // ")\n";
      llvm::outs( ) << "idx: " << " " << " in [" << lower << ", " << upper << ")\n";

      if ( result.find( check_bb ) == result.end( ) ) {
        // 如果check_bb不在result中，直接跳过
        // TODO: result
        //
        // result 是一个map，key是基本块，value是这个基本块的数据流值
        continue;
      }

      // 获取check_bb的数据流值，second是出口值
      auto bb_res = result[ check_bb ].second;
      // 判断idx_val是否在cvmap中，若不是，则说明idx_val不是常数
      // cvmap 是一个map，key是value，value是KSet
      // TODO: cvmap?
      if ( bb_res.cvmap.find( idx_val ) != bb_res.cvmap.end( ) ) {
        llvm::outs( ) << "Index val:" << bb_res.cvmap[ idx_val ]->toString( ) << "\n";
        bool should_remove = true;
        // 判断idx_val是否是top，或者idx_val的所有常数值是否在[lower, upper)中
        // 如果是，则不删除
        if ( bb_res.cvmap[ idx_val ]->top ) {
          should_remove = false;
        } else {
          for ( auto val : bb_res.cvmap[ idx_val ]->const_vals ) {
            if ( val < lower || val >= upper ) {
              should_remove = false;
              break;
            }
          }
        }

        // 如果should_remove为true，则删除check_bb和err_bb
        if ( should_remove ) {
          llvm::outs( ) << "Removing:" << *check_bb << "\n";
          bool pre_removed = false;
          for ( auto it = llvm::pred_begin( check_bb ), et = llvm::pred_end( check_bb );
                it != et; ++it ) {
            // if ( JJY_DEBUG_OPT ) {
            //   // 打印前驱基本块的指令
            //   llvm::outs( ) << "Predecessor: " << **it << "\n";
            // }
            auto pred     = *it;
            auto last_ins = pred->getTerminator( );
            if ( llvm::BranchInst *jmp_ins =
                     llvm::dyn_cast<llvm::BranchInst>( last_ins ) ) {
              jmp_ins->setSuccessor( 0, ok_bb );
              pre_removed = true;
              // if ( JJY_DEBUG_OPT ) {
              //   // 打印前驱基本块的指令
              //   llvm::outs( ) << "Predecessor: " << **it << "\n";
              // }

              break;
            }
          }

          assert( pre_removed && "The last instruction of the previous basic block is "
                                 "not a branch instruction." );

          check_bb->eraseFromParent( );
          err_bb->eraseFromParent( );
        }
      }
    } else {
      assert( 0 && "Uppper or lower is not constant value." );
    }
  }
}
