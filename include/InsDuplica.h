//-----Developed by Jing Yu -------------//
// InsDuplica.h                          //
//=======================================//
//Duplicate all instructions             //
//future work: implement GSR             //
//=======================================//

#ifndef INSDUPLICA_H
#define INSDUPLICA_H

#include <llvm/Pass.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils/Local.h>
#include <llvm/ADT/Statistic.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Module.h>
#include <llvm/Analysis/Dominators.h>

#include "RedundOPT.h"
#include "SafeRegOPT.h"

#include <set>
#include <string>
#include <list>
#include <map>
#include <iostream>
#include <sstream>

#undef Jing_DEBUG 
//#define Jing_DEBUG

using namespace llvm;

namespace llvm {

   class InsDuplica : public FunctionPass  {
      public:
         static char ID;
         InsDuplica():FunctionPass(ID){}
         void getAnalysisUsage (AnalysisUsage &AU) const ;

         bool runOnFunction(Function &F);


      private:
      protected:
         int tmpld;
         int localnuminsdup; //number of generated instructions for this function
         int localnumBBchecker;//number of generated branch checker BBs
         int localnumStorechecker; //number of generated store checker BBs

         //for redundant check
         int localnumfinalldcheck;
         int localnumfinalstcheck;
         int localnumfinalbrcheck;
         int localnumfinalothercheck;

         //for advanced register safe
         int localnumadvregcheckld;
         int localnumadvregcheckst;
         int localnumadvregcheckbr;
         int localnumadvregcheckother;

         BasicBlock *errorBlock;
         //      std::set<std::string> ldnameset;

         void DuplicaAllBB (Function &F);
         virtual void DuplicaBB(BasicBlock*);
         void DuplicaBr(BasicBlock*, Instruction*, BranchInst*);
         void DuplicaInst(Instruction*, Instruction*);
         BasicBlock* DuplicaLoad(LoadInst*, BasicBlock *BB);

         void replaceOperands(Instruction *);
         BasicBlock * newCheckerBB(Instruction*, BranchInst*,BasicBlock*, BasicBlock*,bool);
         BasicBlock* newCheckerSynch(Instruction*,BasicBlock*, Instruction * &nextI);
         virtual BasicBlock *newCheckerStore(Instruction*,BasicBlock*, Instruction * &nextI);
         BasicBlock *newOneValueChecker(Value*, Instruction*, BasicBlock*, std::string&nameTag);

         BasicBlock * buildErrorBlock(Function &F);
         bool notdummyFunc(Function &F); //test if this function is dummy
         bool workFunc(Function &F); //test if this function is in our working set
         std::map<Value*, Value*> valueMap;
         std::map<Instruction*, std::list<Instruction*>*> toAddvalueMap;
         void updateUsersMap(Instruction*, Instruction*);
         void requestToMap(Instruction*,Instruction*);

         bool duplicable(Value*);

         bool isBranchCond(Instruction*);
         Instruction *findLastCond (BasicBlock*);

         void updatePHInodesBB(BasicBlock *, BasicBlock *, BasicBlock *);
         BranchInst *hasConditionalBr(BasicBlock*);

         void initLocalCounter();
         void counterdump(Function&);

         //   friend class InsDuplicaTile;
         bool isSynchPoint(Instruction*);

         std::set<Value*> arguSet;
         bool isFuncArgu(Value*);
         void dupFuncArgu(Function &F);
         void statRegRemove(Instruction*);  // count removal checks for reg safe

         //For redundant checks analysis
         CheckCodeMap *mycheckCodeMap;
         ValueCheckedAtMap *myvalueCheckedAtMap;

         //For advanced register safe optimization
         SafeRegMap *safeRegMap;

         SafeRegforBB *curSafeRegs;  // safe reg sets for current BB

   }; //end of class InsDuplica

   class InsDuplicaTile: public InsDuplica {   
      protected:
         virtual void DuplicaBB(BasicBlock *);
         Instruction* findNextSynchPoint(Instruction*, Instruction*);
   };


}//end of namespace


#endif  //INSDUPLICA_H

// vim: ts=3 sts=3 sw=3 et
