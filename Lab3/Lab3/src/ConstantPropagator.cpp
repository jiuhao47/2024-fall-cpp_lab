#include "ConstantPropagator.h"

char ConstValuePass::ID = 0;
int KSet::kset_size     = 16;

inline llvm::raw_ostream &operator<<( llvm::raw_ostream &out, const ConstValueMap &cv )
{
  // TODO: Implement if you want to print ConstValueMap with something like: llvm::outs()
  // << cv;
}

inline llvm::raw_ostream &operator<<( llvm::raw_ostream &out,
                                      const ConstValueState &state )
{
  // TODO: Implement if you want to print ConstValueState with something like:
  // llvm::outs() << state;
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
  // TODO: hanldes different kinds of instructions
}

void ConstantPropagatorVisitor::merge( ConstValueState *dest, ConstValueState *src )
{
  // TODO
}

void ConstantPropagatorVisitor::initGlobal( ConstValueState *state )
{
  // TODO
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

  removeRedundant( );

  return true;
}

void ConstValuePass::removeRedundant( )
{
  // TODO: You may need to adjust this to fit your implementation.
  llvm::outs( ) << "============== Remove Start ==============\n";
  for ( auto check_bb : check_redundant ) {
    auto br_ins = check_bb->getTerminator( );
    auto ok_bb  = br_ins->getSuccessor( 0 );
    auto err_bb = br_ins->getSuccessor( 1 );

    llvm::Instruction *cond_val =
        llvm::dyn_cast<llvm::Instruction>( br_ins->getOperand( 0 ) );
    llvm::Instruction *upper_cmp =
        llvm::dyn_cast<llvm::Instruction>( cond_val->getOperand( 0 ) );
    llvm::Instruction *lower_cmp =
        llvm::dyn_cast<llvm::Instruction>( cond_val->getOperand( 1 ) );

    llvm::Value *idx_val      = upper_cmp->getOperand( 0 );
    llvm::Value *uppper_bound = upper_cmp->getOperand( 1 );
    llvm::Value *lower_bound  = lower_cmp->getOperand( 1 );

    // idx belongs to [lower, upper)
    if ( llvm::isa<llvm::ConstantInt>( uppper_bound ) &&
         llvm::isa<llvm::ConstantInt>( lower_bound ) ) {
      int upper = llvm::dyn_cast<llvm::ConstantInt>( uppper_bound )->getSExtValue( );
      int lower = llvm::dyn_cast<llvm::ConstantInt>( lower_bound )->getSExtValue( );
      llvm::outs( ) << "idx: " << *idx_val << " in [" << lower << ", " << upper << ")\n";

      if ( result.find( check_bb ) == result.end( ) ) {
        continue;
      }

      auto bb_res = result[ check_bb ].second;
      if ( bb_res.cvmap.find( idx_val ) != bb_res.cvmap.end( ) ) {
        llvm::outs( ) << "Index val:" << bb_res.cvmap[ idx_val ]->toString( ) << "\n";
        bool should_remove = true;
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

        if ( should_remove ) {
          llvm::outs( ) << "Removing:" << *check_bb << "\n";
          bool pre_removed = false;
          for ( auto it = llvm::pred_begin( check_bb ), et = llvm::pred_end( check_bb );
                it != et; ++it ) {
            auto pred     = *it;
            auto last_ins = pred->getTerminator( );
            if ( llvm::BranchInst *jmp_ins =
                     llvm::dyn_cast<llvm::BranchInst>( last_ins ) ) {
              jmp_ins->setSuccessor( 0, ok_bb );
              pre_removed = true;
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
