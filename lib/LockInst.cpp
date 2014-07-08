#include "LockInst.h"
#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <sstream>
#include <iostream>
#include <list>
using namespace std;
using namespace llvm;

char Lock::ID=0;
static RegisterPass<Lock> X("Lock","Lock and Unlock the instructions");
string getString(int tmp)
{
    stringstream newstr;
    newstr<<tmp;
    return newstr.str();
}
string judgeType(Type* ty)
{
    string name="";
    unsigned tyid=ty->getTypeID();

    if(tyid==14)
    {
       tyid=ty->getPointerElementType()->getTypeID();
       name=getString(tyid)+"p.";
    }
    else
       name=getString(tyid)+".";
    return name;
}
string getFuncName(Instruction* I,SmallVector<Type*,8>& opty)
{
    string funcname="";
    funcname+=judgeType(I->getType());
    for(unsigned i=0;i < opty.size();i++)
    {
        funcname+=judgeType(opty[i]);
    }
    return funcname;
}
void Lock::lock_inst(Instruction *I)
{
   LLVMContext& C = I->getContext();
   Module* M = I->getParent()->getParent()->getParent();
   SmallVector<Type*, 8> OpTypes;
   SmallVector<Value*, 8> OpArgs;
   for(Instruction::op_iterator Op = I->op_begin(), E = I->op_end(); Op!=E; ++Op){
      OpTypes.push_back(Op->get()->getType());
      OpArgs.push_back(Op->get());
   }
   FunctionType* FT = FunctionType::get(I->getType(), OpTypes, false);
   Instruction* T = NULL;
   MDNode* LockMD = MDNode::get(C, MDString::get(C, "IFDup"));
   string nametmp=getFuncName(I,OpTypes);
   if (LoadInst* LI=dyn_cast<LoadInst>(I))
   {
      Constant* Func = M->getOrInsertFunction("lock.load."+nametmp, FT);
      CallInst* CI = CallInst::Create(Func, OpArgs, "", I);

      unsigned align = LI->getAlignment();
      CI->setMetadata("align."+getString(align), LockMD);
      if(LI->isAtomic())
         CI->setMetadata("atomic", LockMD);
      if(LI->isVolatile())
         CI->setMetadata("volatile", LockMD);
      I->replaceAllUsesWith(CI);
      T = CI;


   }
   else if(StoreInst* SI=dyn_cast<StoreInst>(I))
   {

   }
   else if(CmpInst* CI=dyn_cast<CmpInst>(I))
   {

   }
   else if(BinaryOperator* BI=dyn_cast<BinaryOperator>(I)){

   }
   SmallVector<pair<unsigned int, MDNode*>, 8> MDNodes;
   I->getAllMetadata(MDNodes);
   for(unsigned I = 0; I<MDNodes.size(); ++I){
      T->setMetadata(MDNodes[I].first, MDNodes[I].second);
   }
}

#include <llvm/Support/InstIterator.h>
bool Lock::runOnModule(Module &M)
{
   for(Module::iterator F = M.begin(), FE = M.end(); F!=FE; ++F){
      for(inst_iterator I = inst_begin(F), IE = inst_end(F); I!=IE; ++I){
         if(isa<LoadInst>(&*I))
            lock_inst(&*I);
      }
   }
   return false;
}

/*
void Lock::unlock_inst(Instruction *I)
{
    if (CallInst* CI=dyn_cast<CallInst>(I))
    {
	Function* tmp=CI->getCalledFunction();
	Type::param_iterator i=tmp->getFunctionType().param_begin();
	string fname=tmp->getName().str();
	//list<value> arglist=tmp->getArgumentList();
	
	if(fname.equals("lock.load"))
	{
	    //list<value*>::iterator i=arglist.begin();
	    LoadInst* LI=new LoadInst(**(i));
	    if(**(i+1)==true)
		LI->setAtomic(**(i+4),**(i+5));
	    LI->setVolatile(**(i+2));
	    LI->setAlignment(**(i+3));
	    I=LI;
	}	    
    }
}
*/
