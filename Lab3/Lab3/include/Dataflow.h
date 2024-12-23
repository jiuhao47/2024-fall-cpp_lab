#ifndef _DATAFLOW_H_
#define _DATAFLOW_H_

#include <set>
#include <map>
#include <utility>
#include <iostream>

#include <llvm/IR/Module.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/Support/raw_ostream.h>


// Instruction -> <in state, out state>
template <class T>
struct DataflowInstResult
{
	typedef typename std::map<llvm::Instruction *, std::pair<T, T>> Type;
};

// Dummy class to provide a typedef for the detailed result set
// For each basicblock, we compute its input dataflow val and its output dataflow val
// BasicBlock -> <in state, out state>
template<class T>
struct DataflowBBResult {
    typedef typename std::map<llvm::BasicBlock *, std::pair<T, T>> Type;
};

// Base dataflow visitor class, defines the dataflow function

template <class T>
class DataflowVisitor {
public:
    llvm::Module *module;

    virtual ~DataflowVisitor() {}

    /*
        Dataflow Function invoked for each instruction

        @inst the Instruction
        @dfval the input dataflow value
        @return true if dfval changed
    */
    virtual void compDFVal(llvm::Instruction *inst, T *state) = 0;

    /* 
        Dataflow Function invoked for each basic block 

        @block the Basic Block
        @dfval the input dataflow value
        @isforward true to compute dfval forward, otherwise backward
    */
    virtual void compDFVal(llvm::BasicBlock *block, T *state, bool isforward)
    {
        if (isforward == true) {
            if (block->getName() == "entry" && block->getParent()->getName() == "main") {
                initGlobal(state);
            } 
            for (llvm::BasicBlock::iterator ii=block->begin(), ie=block->end(); 
                ii!=ie; ++ii) {
                llvm::Instruction * inst = &*ii;
                compDFVal(inst, state);
            }
        } else {
            for (llvm::BasicBlock::reverse_iterator ii=block->rbegin(), ie=block->rend();
                ii != ie; ++ii) {
                llvm::Instruction * inst = &*ii;
                compDFVal(inst, state);
            }
        }
    }

    /*
        Merge of two dfvals, dest will be the merged result
        @return true if dest changed
    */
    virtual void merge( T *dest, T *src )
    {}

    virtual void initGlobal(T *state)
    {}
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
template<class T>
void compForwardDataflow(llvm::Function *fn, DataflowVisitor<T> *visitor, typename DataflowBBResult<T>::Type *result, const T & initval) {
    // TODO: Implement forward dataflow analysis framework.
}

template<class T>
void compBackwardDataflow(llvm::Function *fn,DataflowVisitor<T> *visitor, typename DataflowBBResult<T>::Type *result, const T &initval)
{}

#endif