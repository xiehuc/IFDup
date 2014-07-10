#ifndef _LOCK_INST_H_H
#define _LOCK_INST_H_H
#include <llvm/Pass.h>
#include <llvm/IR/Instruction.h>

class Lock:public llvm::ModulePass
{
   public:
	static char ID;
	Lock():ModulePass(ID){}
	void getAnalysisUsage(llvm::AnalysisUsage& AU) const
	{
	    AU.setPreservesAll();
	}
	bool runOnModule(llvm::Module& M);
	void lock_inst(llvm::Instruction* I);
};

class Unlock:public llvm::ModulePass
{
   public:
	static char ID;
	Unlock():ModulePass(ID){}
	void getAnalysisUsage(llvm::AnalysisUsage& AU) const
	{
	    AU.setPreservesAll();
	}
	bool runOnModule(llvm::Module& M);
	void unlock_inst(llvm::Instruction* I);
};
#endif
