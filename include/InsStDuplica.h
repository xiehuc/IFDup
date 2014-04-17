//-------------------Developed by Jing Yu----------------//
//InsStDuplica.h                                         //
//====================================================== //

#ifndef INSSTDUPLICA_H
#define INSSTDUPLICA_H

#include "InsDuplica.h"
#include "StoreVAtool.h"

using namespace llvm;

namespace llvm {
    class InsStDuplica: public InsDuplica {
    public:
        virtual bool runOnFunction(Function &F);
        virtual void getAnalysisUsage (AnalysisUsage &AU) const {
            AU.addRequired<LoopInfo>();
        }

	void DuplicaAllBB(Function &F,LoopInfo &loopinfo);
	StoreVAtool *stVAtool;
	void stdupBB(BasicBlock *);    
	void recordSynch(Instruction*, BasicBlock*,std::list<checkVec*>*);
    	void propSigtoNewBr(BasicBlock*);
        void requestAllToMap(UpdateListType*);
    }; //end of class InsStDuplica



} //end of namespace llvm


#endif //INSSTDUPLICA_H
