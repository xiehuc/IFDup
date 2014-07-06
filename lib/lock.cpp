#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>

#include <debug.h>
#include <list>
using namespace std;
using namespace llvm;

void lock_inst(Instruction *I){
    if (LoadInst* LI=dyn_cast<LoadInst>(I)){
	Value* argu[6];
	if(LI->isAtomic())
	    argu[0]=true;
	else
	    argu[0]=false;
	if(LI->isVolatile())
	    argu[1]=true;
	else
	    arg[1]=false;
	argu[2]=&(LI->getPointerOperand());
	argu[3]=&(LI->getAlignment());
	argu[4]=&(LI->getOrdering());
	argu[5]=&(LI->getSynchScope());
	ArrayRef<Value*>array=ArrayRef(&argu,6);
	Function* New=Function.Create(FunctionType.get(Type::VoidTyID,array,false),GlobalValue::ExternalLinkage,"lock.load");	
	New->setName("lock.load");
	CallInst* tmp=CallInst.Create(New);
	//tmp->setCalledFunction(New);
	I=tmp;
    }
    else if(StoreInst* SI=syn_cast<StoreInst>(I)){
    
    }
    else if(CmpInst* CI=syn_cast<CmpInst>(I)){

    }
    else if(BinaryOperator* BI=syn_cast<BinaryOperator>(I)){
    	
    }
}

void unlock_inst(Instruction *I){
    if (CallInst* CI=dyn_cast<CallInst>(I)){
	Function* tmp=CI->getCalledFunction();
	Type::param_iterator i=tmp->getFunctionType().param_begin();
	StringRef fname=tmp->getName();
	//list<value> arglist=tmp->getArgumentList();
	
	if(fname.equals("lock.load")){
	    //list<value*>::iterator i=arglist.begin();
	    LoadInst* LI=new LoadInst(**(i+2));
	    if(**i==true)
		LI->setAtomic(**(i+4),**(i+5));
	    LI->setVolatile(**(i+1));
	    LI->setAlignment(**(i+3));
	    I=LI;
	}	    
    }
}
