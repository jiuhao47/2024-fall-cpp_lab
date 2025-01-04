#ifndef _DATAFLOW_H_
#define _DATAFLOW_H_

#include "../src/Utils.h"
#include <iostream>
#include <map>
#include <set>
#include <utility>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

// Instruction -> <in state, out state>
template <class T> struct DataflowInstResult
{
  typedef typename std::map<llvm::Instruction *, std::pair<T, T>> Type;
};

// Dummy class to provide a typedef for the detailed result set
// For each basicblock, we compute its input dataflow val and its output dataflow val
// BasicBlock -> <in state, out state>
template <class T> struct DataflowBBResult
{
  typedef typename std::map<llvm::BasicBlock *, std::pair<T, T>> Type;
};

// Base dataflow visitor class, defines the dataflow function

template <class T> class DataflowVisitor
{
public:
  llvm::Module *module;

  virtual ~DataflowVisitor( ) {}

  /*
      Dataflow Function invoked for each instruction

      @inst the Instruction
      @dfval the input dataflow value
      @return true if dfval changed
  */
  virtual void compDFVal( llvm::Instruction *inst, T *state ) = 0;

  /*
      Dataflow Function invoked for each basic block

      @block the Basic Block
      @dfval the input dataflow value
      @isforward true to compute dfval forward, otherwise backward
  */
  virtual void compDFVal( llvm::BasicBlock *block, T *state, bool isforward )
  {
    if ( JJY_DEBUG_OPT ) {
      llvm::outs( ) << JJY_DEBUG_SIGN << " " << __FILE_NAME__ << " " << __func__ << " "
                    << __LINE__ << "\n";
    }
    if ( isforward == true ) {
      if ( block->getName( ) == "entry" && block->getParent( )->getName( ) == "main" ) {
        initGlobal( state );
      }
      for ( llvm::BasicBlock::iterator ii = block->begin( ), ie = block->end( ); ii != ie;
            ++ii ) {
        llvm::Instruction *inst = &*ii;
        compDFVal( inst, state );
      }
    } else {
      for ( llvm::BasicBlock::reverse_iterator ii = block->rbegin( ), ie = block->rend( );
            ii != ie; ++ii ) {
        llvm::Instruction *inst = &*ii;
        compDFVal( inst, state );
      }
    }
  }

  /*
      Merge of two dfvals, dest will be the merged result
      @return true if dest changed
  */
  virtual void merge( T *dest, T *src ) {}

  virtual void initGlobal( T *state ) {}
};

/*
    Compute a forward iterated fixedpoint dataflow function, using a user-supplied
    visitor function. Note that the caller must ensure that the function is
    in fact a monotone function, as otherwise the fixedpoint may not terminate.

    @param fn The function
    @param visitor A function to compute dataflow vals
    @param result The results of the dataflow
    @initval the Initial dataflow value
*/
template <class T>
void compForwardDataflow( llvm::Function *fn, DataflowVisitor<T> *visitor,
                          typename DataflowBBResult<T>::Type *result, const T &initval )
{
  if ( JJY_DEBUG_OPT ) {
    llvm::outs( ) << JJY_DEBUG_SIGN << " " << __FILE_NAME__ << " " << __func__ << " "
                  << __LINE__ << "\n";
  }
  // TODO: Implement forward dataflow analysis framework.
  //
  std::vector<llvm::BasicBlock *> worklist;

  // 初始化
  for ( llvm::BasicBlock &BB : *fn ) {
    if ( JJY_DEBUG_OPT ) {
      // print BB name
      llvm::outs( ) << "------------init\n";
      llvm::outs( ) << JJY_DEBUG_SIGN << " BB: " << BB.getName( ) << "\n";
      // show worklist content
      llvm::outs( ) << JJY_DEBUG_SIGN << " worklist:\n";
      for ( llvm::BasicBlock *bb : worklist ) {
        llvm::outs( ) << "\t" << bb->getName( ) << "\n";
      }
    }
    // if ( BB.getName( ).startswith( "obc_err_" ) ) {
    //   if ( JJY_DEBUG_OPT ) {
    //     llvm::outs( ) << JJY_DEBUG_SIGN << " Skip obc_err_\n";
    //   }
    //   continue;
    // }
    T init_state;
    if ( &BB == &fn->getEntryBlock( ) ) {
      ( *result )[ &BB ].first = initval;
    } else {
      visitor->initGlobal( &( *result )[ &BB ].first );
    }
    visitor->initGlobal( &( *result )[ &BB ].second );
    worklist.push_back( &BB );
  }

  // 迭代计算
  while ( !worklist.empty( ) ) {
    llvm::BasicBlock *BB = worklist.back( );
    worklist.pop_back( );

    if ( JJY_DEBUG_OPT ) {
      llvm::outs( ) << "------------calc\n";
      llvm::outs( ) << JJY_DEBUG_SIGN << " BB: " << BB->getName( ) << "\n";
      // print all inst
      for ( llvm::Instruction &inst : *BB ) {
        llvm::outs( ) << inst << "\n";
      }
    }
    // if ( BB->getName( ).startswith( "obc_err_" ) ) {
    //   if ( JJY_DEBUG_OPT ) {
    //     llvm::outs( ) << JJY_DEBUG_SIGN << " Skip obc_err_\n";
    //   }
    //   // 虽然跳过了obc_err_，但是要将此块的输入输出数据流与其前驱的输出数据流合并

    //   // 合并前驱的输出值

    //
    //   continue;
    // }

    // 合并前驱的输出值
    T in;
    visitor->initGlobal( &in );

    if ( BB == &fn->getEntryBlock( ) ) {
      in = initval;
    } else {
      for ( auto pred = pred_begin( BB ); pred != pred_end( BB ); ++pred ) {
        visitor->merge( &in, &( *result )[ *pred ].second );
      }
    }

    ( *result )[ BB ].first = in;

    // 计算当前基本块的输出值
    T out                   = in; // 从输入值开始
    if ( BB->getName( ).startswith( "obc_err_" ) ) {
      // 如果是obc_err_，直接将输入值赋给输出值
      out = in;
    } else {
      visitor->compDFVal( BB, &out, true );
    }
    // visitor->compDFVal( BB, &out, true );

    if ( JJY_DEBUG_OPT ) {
      // show in and out
      llvm::outs( ) << JJY_DEBUG_SIGN << " in:\n" << in << "\n";
      llvm::outs( ) << JJY_DEBUG_SIGN << " out:\n" << out << "\n";
    }

    // 如果输出值改变，更新并添加后继到工作表
    if ( !( out == ( *result )[ BB ].second ) ) {
      ( *result )[ BB ].second = out;
      for ( auto succ = succ_begin( BB ); succ != succ_end( BB ); ++succ ) {
        worklist.push_back( *succ );
      }
    }
    if ( JJY_DEBUG_OPT ) {
      llvm::outs( ) << JJY_DEBUG_SIGN << " worklist after push :\n";
      for ( llvm::BasicBlock *bb : worklist ) {
        llvm::outs( ) << "\t" << bb->getName( ) << "\n";
      }
    }
  }
}

template <class T>
void compBackwardDataflow( llvm::Function *fn, DataflowVisitor<T> *visitor,
                           typename DataflowBBResult<T>::Type *result, const T &initval )
{
}

#endif
