//-------------Developed by Jing Yu ----------//
//StoreVAtool.h                               //
//============================================//
//Provide tools for store signature checking  //
//============================================//

#ifndef STOREVATOOL_H
#define STOREVATOOL_H

#include "llvm/Pass.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Function.h"
#include "llvm/Instructions.h"
#include "llvm/Instruction.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Module.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/CFG.h"
#include "llvm/InstrTypes.h"
#include "llvm/Type.h"

#include <map>
#include <set>
#include <list>
#include <utility>
#include <string>
#include <iostream>

#define AddressType  Type::UIntTy
//Type::ShortTy 
// Type::UIntTy
#define ValueType Type::IntTy
// Type::ShortTy
// Type::IntTy


using namespace llvm;

namespace llvm {

typedef std::pair<Value*,Value*> ValuePairType;
typedef std::pair<StoreInst*,ValuePairType*> checkVec;
typedef std::pair<Instruction*,Instruction*> IPairType;
typedef std::list<IPairType*> UpdateListType;

    struct LoopBBStSig {
     public:
	//construct
	LoopBBStSig();
	~LoopBBStSig();
	LoopBBStSig(Value*,Value*); //set_A, set_V
	void init();
	Value *in_StA, *in_StV, *out_StA, *out_StV;
	//Instruction *firstUseofStA, *firstUseofStV;
	bool insideLoop;

	Instruction *firstUseA, *firstUseV;
	ConstantInt *makeZero (const Type *);	
	void dump();
    };//struct LoopBBStSig

    ///////class storeVAtool //////
    class StoreVAtool {
     public:
	//construct
	inline StoreVAtool();
	StoreVAtool(LoopInfo *loopinfo, Function &F);
	~StoreVAtool();
	bool findInterestingLoop(float s = 1);
	bool isInteresting(BasicBlock*);
	bool buildMyInSig (BasicBlock*, bool, const Type*);
	UpdateListType * checkStores(std::list<checkVec*>*,BasicBlock*);
	void propSig (BasicBlock*,BasicBlock*); //inherit parent's sig
	void updateSucSigs (BasicBlock*);
	void addSigCheckers(BasicBlock *errorBlock);
        UpdateListType* processStlist(std::list<checkVec*>*,BasicBlock*); 
	void dumpStat();
	void clean();
    private:
	LoopInfo *funcloop;
	Function *myfunc;
	std::map<BasicBlock*,LoopBBStSig*> BBStSigMap;
        std::map<Loop*,std::vector<BasicBlock*>*> ExitBBList;
	std::set<Loop*> LoopList; //contains all loops we are interested in
	std::map<Loop*,const Type*> valueTypeMap;
	float ManyStore; //specify the condition judging which loop is interesting for us. Default is 1 store/basicBlock
	ConstantInt *errorValue;

	void init();	
        ConstantInt *makeZero (const Type *);

	bool workOnIt(Loop *inLoop, const Type* &myType);
	void InitPreHeaders(Loop*);
	void insertSigtoMap(BasicBlock *, LoopBBStSig*);
	LoopBBStSig * findStSigfor(BasicBlock*);
	Value *findStVfor (BasicBlock*BB,BasicBlock*curBB, const Type*myType=ValueType);
	Value *findStAfor (BasicBlock*BB);
	
	Instruction* newOneSigCal(Value* V,Value* V_r,Instruction* I,std::string nametag, Value* &stSig);
	void updateMySig (BasicBlock *,BasicBlock *,LoopBBStSig*);
	void addSigCheckforLoop (Loop*, BasicBlock*);
	void addSigCheckforBB(BasicBlock*, BasicBlock*);

        int localnewCast, localStSigChecker, localStSigCal;
	void recordExitBB (Loop*);
	void replaceSigUseIn(Instruction *I, Value*oldV, Value*newV);

	void updatePHInodesBB(BasicBlock*,BasicBlock*,BasicBlock*);
	bool hasInductionVar(Value*, Loop*);
    };//class StoreVAtool
    


 static Statistic<> NumNewCast("numNewCast", "Number of new CastInst");
 static Statistic<> NumStSigChecker("numStSigChecker","number of Store signature checkers");
 static Statistic<> NumStSigCal("numStSigCal","number of Store signature calculations");
}// namespace llvm


#endif  //STOREVATOOL_H
