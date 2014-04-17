/////////////////////////////////////////
//RedundOPT.h                          //
/////////////////////////////////////////
//OPtimizations for redundant check removal.//
//Class are implemnetd in RedundAnalysis.cpp//
//and RedundRemoval.cpp                     //
//////////////////////////////////////////////

#ifndef REDUNDOPT_H
#define REDUNDOPT_H

#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Function.h>
#include <llvm/ADT/Statistic.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/Analysis/Dominators.h>
#include <llvm/Analysis/LoopInfo.h>

#include <llvm/Support/CFG.h>
#include <llvm/IR/Constants.h>

#include <llvm/Support/raw_ostream.h>

#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <string>
#include <list>

using namespace llvm;

namespace llvm { 
    ////////////////////////////////////
    // Class CheckCode                //
    ////////////////////////////////////

    class CheckCode {
    public:
	CheckCode(Instruction* I) {
	    CheckCodeInst = I;
	    CheckElem.clear();
	    origNumElem = 0;
	}
	virtual ~CheckCode() {CheckElem.clear();}

    protected:
	Instruction *CheckCodeInst;
	std::set<Value*> CheckElem;
	unsigned int origNumElem;
	
    public:
	virtual unsigned int getOrigNumElem() {return origNumElem;}
	void insertOrigElement(Value*);
	void deleteElement(Value*);
	virtual std::set<Value*>& getCheckElemList(){return CheckElem;} //final version
	std::set<Value*>& getCheckElems() {return CheckElem;}//whatever in CheckElem
	virtual unsigned int getFinalNumElem(){return CheckElem.size();}
	void dumpCheckCode();
	virtual void dump();
    }; //end of CheckCode


    ////////////////////////////////////
    class CheckLoad : public CheckCode {
    public: 
	CheckLoad(Instruction*);
	virtual ~CheckLoad();
    private:
	std::set<Value*> finalElems;
	bool computeFinal;
	void ComputeFinal(); //compute finalElems
    public:
	virtual unsigned int getOrigNumElem();
	virtual std::set<Value*>& getCheckElemList();
	virtual unsigned int getFinalNumElem();
	virtual void dump();
    }; //end of CheckLoad

    //////////////////////////////////////
    class CheckStore : public CheckCode{
    public:
	CheckStore(Instruction*);
	virtual ~CheckStore();
    private:
	std::set<Value*> finalElems;
	bool computeFinal;
	void ComputeFinal();
    public:
	virtual unsigned int getOrigNumElem();
	virtual std::set<Value*>& getCheckElemList();
	virtual unsigned int getFinalNumElem();
	virtual void dump();
    }; //end of CheckStore


    //////////////////////////////////////
      class CheckBranch : public CheckCode {
       public:
	CheckBranch(Instruction*);
	virtual  ~CheckBranch();
	
      private:
	std::vector<Value*> propCheckList;
	std::vector<bool> propToList;
	std::vector<bool> isOrigList; //not in use. always false
	
      public:
	virtual unsigned int getOrigNumElem();
	virtual std::set<Value*>& getCheckElemList();
	virtual unsigned int getFinalNumElem();
	virtual void dump();

	void insertPropCheck(Value* elem, bool propTo, bool orig){
	    int s = propCheckList.size();
	    int i = 0;
	    while (i < s && propCheckList[i] != elem) i++;
	    if (i == s) { //not in list
		propCheckList.push_back(elem);
		propToList.push_back(propTo);
		isOrigList.push_back(orig);
	    } else { //already in list. Error!
		if (propToList[i] != propTo) {
		    assert(0 && "Elem already exsits with different prop direction\n");
		}
	    }
	}
	unsigned int PropCheckSize() {return propCheckList.size();}
	Value *getPropCheckValue(unsigned int t) { return propCheckList[t];}
	bool getPropTo(unsigned int t){return propToList[t];}
	bool getPropOrig(unsigned int t) {return isOrigList[t];}
	
	void dump_propCheck() {
		unsigned int s = propCheckList.size();
		errs() << "propCheckList:";
		for (unsigned int i = 0; i < s; i++) {
			errs() << "<"<<propCheckList[i]->getName()<<",br("<<
				propToList[i]<<"),Orig("<< isOrigList[i]<<")>";		
		}
	}
	
      }; //end of class CheckBranch


      ////////////////////////////////
      // class CheckCodeMap         //
      ////////////////////////////////
      class CheckCodeMap {
      public:
	  CheckCodeMap(){checkCodeMap.clear();}
	  ~CheckCodeMap();
      private:
	  std::map<Instruction*, CheckCode*> checkCodeMap;

      public:
	  CheckCode* getCheckCode(Instruction*); //return null if not found
	  CheckCode* newCheckCode(Instruction*);
	  void deleteElem(Instruction*, Value*);
	  std::map<Instruction*,CheckCode*>& getMap() {return checkCodeMap;}
	  std::set<Value*> *getCheckElemList(Instruction*);
	  bool empty() {return checkCodeMap.empty(); }
	  void dump();
      }; //end of CheckCodeMap


      /////////////////////////////////
      //Class ValueCheckedAt         //
      /////////////////////////////////
      class ValueCheckedAt {
      public:
	  ValueCheckedAt(Value *myElem) {
	      CheckElem = myElem;
	      CheckedAtList.clear();
	      PropOrFinalList.clear();
	  };
	  ~ValueCheckedAt() { 
	      CheckedAtList.clear();
	      PropOrFinalList.clear();
	  }
	  
      private:
	  Value * CheckElem;
      public:
	  std::set<Instruction*> CheckedAtList;
	  std::set<Instruction*> PropOrFinalList;
	  
	  bool notRemovable() {return CheckedAtList.empty();}
	  void insertCheckedAt(Instruction*I) {CheckedAtList.insert(I);}
	  void insertPropOrFinal(Instruction*I) {PropOrFinalList.insert(I);}
	  bool propagateTo(ValueCheckedAt *laterCheck);
	  void dump();

      }; //end of ValueCheckedAt

      
      ////////////////////////////////
      //Class ValueCheckedAtMap     //
      ////////////////////////////////
      class ValueCheckedAtMap {
      public:
	  ValueCheckedAtMap() {valueCheckedAtMap.clear();}
	  ~ValueCheckedAtMap();
      private:
	  std::map<Value*, ValueCheckedAt*> valueCheckedAtMap;

      public:
	  std::map<Value *, ValueCheckedAt*>& getMap() {
	      return valueCheckedAtMap;
	  }
	  void insertCheckedAt(Value *elem, Instruction *I);
	  void insertPropOrFinal(Value *elem, Instruction *I);
	  bool contain(Value*);
	  ValueCheckedAt *getValueCheckedTable(Value* elem); //return a new one, if not found
	  bool empty() {return valueCheckedAtMap.empty();}
	  void shrink(std::set<Value*> &keystokeep);
	  void getAllKeys(std::set<Value*> &allkeys);
	  void dump();
 
      };// end of ValueCheckedAtMap


      ///////////////////////////////////
      //Class LoopIVInfo               //
      ///////////////////////////////////
      //record IV information for a loop
      class LoopIVInfo {
      public:
	  LoopIVInfo(Loop*);
	  ~LoopIVInfo();

	  std::set<Value*> IVset;
	  std::map<BranchInst*,bool> ExitingBranch;

	  void addExitingBlocks(std::vector<BasicBlock*>&blocks);
	  void addIVset(std::vector<PHINode*>&IVs);
	  void dump();

      private:
	  Loop *myloop;
	  void addBranch(BranchInst*, bool);
	  void addExitingBlock(BasicBlock*);
      };


      /////////////////////////////////////
      /////////////////////////////////////
      //  RedundAnalysis class           //
      /////////////////////////////////////

      enum CHECKTYPE {
	  PROPORFINAL,
	  CHECKEDAT
	  };

      class RedundAnalysis {
      public:
	  RedundAnalysis();
	  ~RedundAnalysis();

	  //interface
	  void SetUpTable(CheckCodeMap *checkCodeMap, ValueCheckedAtMap *valueCheckedAtMap, Function &F);
	  void removeOverlap(CheckCodeMap *checkCodeMap, ValueCheckedAtMap *valueCheckedAtMap, Function &F, PostDominatorSet &postdominSet);
	  void rmSafeReg(CheckCodeMap *checkCodeMap, ValueCheckedAtMap *valueCheckedAtMap, Function &F);
	  void printStatforTotal(Function &F);
	  void rmLoopIV(CheckCodeMap *checkCodeMap, ValueCheckedAtMap *valueCheckedAtMap, Function &F, LoopInfo &loopinfo);
	  void checkADVRegSafe(DominatorSet *dominset);


      private:
	  CheckCodeMap *MycheckCodeMap;
	  ValueCheckedAtMap *MyvalueCheckedAtMap;
	  Function *MyF;

      private: 
	  //for scan program. 
	  //Ugly: some are overlapped with InsDuplica class
	  bool duplicable(Value *); //overlap
	  bool isFuncArgu(Value*);  //overlap
	  Instruction *findLastCond(BasicBlock*); //overlap
	  BranchInst *hasConditionalBr(BasicBlock *BB); //overlap
	  void setupFuncArguSet(Function &F);
	  std::set<Value*> arguSet;

	  //for table setup
	  std::list<Instruction*>ToUpdateList;
	  void addtoUpdateList(Value*);
	  void ScanAllCheckCodes(Function &F);
	  void PropagateChecks();
	  bool canPropErrorInst(Instruction*);
	  bool isCheckPoint(Instruction*ins);
	  void SetupTablewithCheckPoint(Instruction*, BasicBlock*);
	  void SetupTablewithBranch(Instruction *, BranchInst*, BasicBlock*);
	  void SetupTablewithReturn(ReturnInst *, BasicBlock*);
	  void SetupTablewithLoad(LoadInst *, BasicBlock*);
	  void SetupTablewithStore(StoreInst *, BasicBlock*, enum CHECKTYPE);
	  void SetupTablewithCall(CallInst*, BasicBlock*, enum CHECKTYPE);
	  void SetUpTablewithOP(CheckCode*, Value*, Instruction*,enum CHECKTYPE);
	  //for remove overlap
	  bool removeOverlapOnValue(Value*v,ValueCheckedAt*checkatTable,PostDominatorSet &postdominSet);

	  //for load address decomposition
	  void DecomposeAddress(std::set<Value*>&valueSet, Value *addrP);
	  Value *scanCast(Value*);
	  
	  //for regsafe
	  bool isSafeReg(Value*);
	  void statRegRemove(Instruction *, int);
	  void printSafeRegPassStat(Function &F);

	  //for advanced reg safe
	  bool reg_safe;
	  DominatorSet *DominSet;

	  //for loop iv
	  void getInnermostLoop(std::list<Loop*>&innermostLoops, Function &F,LoopInfo &loopinfo);
	  LoopIVInfo *getLoopIV(Loop*);
	  bool optimizeOnIV(Loop*,LoopIVInfo*);
	  void getAllInductionVariable(Loop*loop, std::vector<PHINode*>&IVs);
	  bool insideLoop(Loop*, Instruction*);
	  void moveOutofLoop(Value*, LoopIVInfo*);
	  void optConstantCheck(Value*,Loop *,Instruction*,LoopIVInfo*);
	  void statLoopOpt(Value*, Instruction *);
	  void printLoopOptPassStat(Function &F);

	  //for synchpoint
	  std::vector<char> connectTable; //connectivity
	  std::vector<char> dirtyTable;  //has a dirty path
	  int BBtotalN; //num of BBs
	  std::vector<int> DirtyBBID;
	  std::set<BasicBlock*> DirtyBBset;
	  std::map<BasicBlock*, int> BBIDmap;

	  void buildSynchPointTable();
	  void buildCTable();
	  void buildDTable();
	  void addPathtoCTable(BasicBlock*, BasicBlock*);
	  void addPathFromBB(BasicBlock*);
	  void markBBdirty(BasicBlock*);
	  int getBBID(BasicBlock*);
	  void initSynchPoint(Function &F);	  
	  bool hasSynchPoint(Loop*);
	  bool hasSynchPoint(BasicBlock*);
	  bool hasSynchPoint(BasicBlock*, BasicBlock *);
	  bool hasSynchPoint(Instruction*, Instruction*);
	  bool hasSynchPointWithinBB(Instruction*,Instruction*);
	  bool isSynchPoint(Instruction*I);


	  //statistic
	  int localnumtotalldcheck;
	  int localnumtotalstcheck;
	  int localnumtotalbrcheck;
	  int localnumtotalothercheck;

	  int localnumsaferegld;
	  int localnumsaferegst;
	  int localnumsaferegbr;
	  int localnumsaferegother;

	  int localnumloopld;
	  int localnumloopst;
	  int localnumloopbr;
	  int localnumloopother;


      }; //end of class RedundAnalysis

}//end of namespace


#endif //REDUNDOPT_H
