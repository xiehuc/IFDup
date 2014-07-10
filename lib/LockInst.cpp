#include "LockInst.h"
#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/InstIterator.h>

#include <sstream>

using namespace std;
using namespace llvm;

char Lock::ID=0;
char Unlock::ID=0;
static RegisterPass<Lock> X("Lock","Lock the instructions");
static RegisterPass<Unlock> Y("Unlock","Unlock the locked instructions");

/*将整形转化成string类型*/
static string getString(int tmp)
{
   stringstream newstr;
   newstr<<tmp;
   return newstr.str();
}

/*判断参数类型*/
static string judgeType(Type* ty)
{
   string name="";
   Type::TypeID tyid=ty->getTypeID();
   Type* tmp = ty;
   while(tyid == Type::PointerTyID){
      tmp=tmp->getPointerElementType();
      tyid=tmp->getTypeID();
      name+="p";
   }
   if(tyid==Type::IntegerTyID){
      name=name+getString(tyid)+getString(tmp->getPrimitiveSizeInBits());
   }
   else
      name=getString(tyid);
   return name;
}

/*得到CallInst指令的函数名称*/
static string getFuncName(Instruction* I,SmallVector<Type*,8>& opty)
{
   string funcname="";
   unsigned size=opty.size();
   funcname+=judgeType(I->getType())+((size>0)?".":"");
   unsigned i;
   for(i=0;i < size-1;i++){
      funcname+=(judgeType(opty[i])+".");
   }
   funcname+=judgeType(opty[i]);
   return funcname;
}

/*锁指令函数，将给定的指令转成call指令*/
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
   if (LoadInst* LI=dyn_cast<LoadInst>(I)){
      Constant* Func = M->getOrInsertFunction("lock.load."+nametmp, FT);
      CallInst* CI = CallInst::Create(Func, OpArgs, "", I);

      unsigned align = LI->getAlignment();
      CI->setMetadata("align."+getString(align), LockMD);
      if(LI->isAtomic())
         CI->setMetadata("atomic."+getString(LI->getOrdering()), LockMD);
      if(LI->isVolatile())
         CI->setMetadata("volatile", LockMD);
      I->replaceAllUsesWith(CI);
      
      /*将LoadInst指令的操作数设为UndefValue，不进行这个操作的话remove指令会出错*/
      for(unsigned i =0;i < I->getNumOperands();i++)
         I->setOperand(i, UndefValue::get(I->getOperand(i)->getType()));
      /*删除LoadInst指令*/
      I->removeFromParent();
      T = CI;
   }
   else if(StoreInst* SI=dyn_cast<StoreInst>(I)){

   }
   else if(CmpInst* CI=dyn_cast<CmpInst>(I)){

   }
   else if(BinaryOperator* BI=dyn_cast<BinaryOperator>(I)){

   }
   /*将指令I的MetaData存到T指令的MetaData中，便于后期对I指令的完整恢复*/
   SmallVector<pair<unsigned int, MDNode*>, 8> MDNodes;
   I->getAllMetadata(MDNodes);
   for(unsigned I = 0; I<MDNodes.size(); ++I){
      T->setMetadata(MDNodes[I].first, MDNodes[I].second);
   }
}


bool Lock::runOnModule(Module &M)
{
   return false;
}
bool Unlock::runOnModule(Module &M)
{
   /*解锁被锁住的指令*/
   for(Module::iterator F = M.begin(), FE = M.end(); F!=FE; ++F){
      inst_iterator I = inst_begin(F);
      while(I!=inst_end(F)){
         Instruction* self = &*I;
         // step first, to void memory crash
         I++;
         if(isa<CallInst>(self)){
            unlock_inst(self);
         }
      }
   }
   // remove empty function declare
   Module::iterator F = M.begin();
   while(F!=M.end())
   {
      Function* Ftmp = &*F;
      F++;
      if(Ftmp->getName().find("lock.")==0)
         Ftmp->removeFromParent();
   }
   return false;
}

void Unlock::unlock_inst(Instruction* I)
{
   LLVMContext& C = I->getContext();
   CallInst* CI=cast<CallInst>(I);
   SmallVector<Type*, 8>OpTypes;
   SmallVector<Value*, 8>OpArgs;
   for(Instruction::op_iterator Op = I->op_begin(), E = I->op_end(); Op!=E; ++Op){
      OpTypes.push_back(Op->get()->getType());
      OpArgs.push_back(Op->get());
   }
   Function* F=CI->getCalledFunction();
   string cname=F->getName().str();
   /*获取CI指令的所有MetaData*/
   SmallVector<pair<unsigned int, MDNode*>, 8> MDNodes;
   CI->getAllMetadata(MDNodes);
   /*获取模块中所有MetaData的名称*/
   SmallVector<StringRef, 30> names; 
   C.getMDKindNames(names);
   /*将Load指令解锁*/
   if(cname.find("lock.load") < cname.length()){
      LoadInst* LI=new LoadInst(OpArgs[0],"",I);
      for(unsigned i = 0; i < MDNodes.size(); i++){
         SmallVector<StringRef, 10> tmp;
         errs()<<names[MDNodes[i].first].str()<<"\n";
         names[MDNodes[i].first].split(tmp,".");
         //cerr<<tmp[0]<<"\t"<<endl;
         if(tmp[0].str()=="volatile")
            LI->setVolatile(true);
         else if(tmp[0].str()=="atomic"){
            LI->setAtomic((AtomicOrdering)(atoi(tmp[1].str().c_str())));
         }
         else if(tmp[0].str()=="align")
            LI->setAlignment(atoi(tmp[1].str().c_str()));
         else
            LI->setMetadata(MDNodes[i].first, MDNodes[i].second);
      }
      I->replaceAllUsesWith(LI);
      for(unsigned i =0;i < I->getNumOperands();i++)
      {
         I->setOperand(i, UndefValue::get(I->getOperand(i)->getType()));
      }
      //for(unsigned i = 0; i < MDNodes.size(); i++)
        // MDNodes[i].second->replaceOperandWith(MDNodes[i].first,UndefValue::get(I->getOperand(i)->getType()));
      I->removeFromParent();
      //cerr<<endl;
      errs()<<"found lock.\t"<<cname<<"\n";
   }
   else
      errs()<<"not found lock.\n";
}
