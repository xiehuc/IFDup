//------Developed by Jing Yu --------------//
// IFDuplica.h                             //
//=========================================//
//Duplicate all instructions, give special //
//care to nested ifs                       //
//=========================================//

#ifndef IFDUPLICA_H
#define IFDUPLICA_H

#include "ShortcutConstruct.h"
#include "InsDuplica.h"

#undef Jing_DEBUG1 
//#define Jing_DEBUG1

using namespace llvm;

namespace llvm {
    
    class IfDuplica :public InsDuplica {
    public:
	virtual void getAnalysisUsage (AnalysisUsage &AU) const {
	    AU.addRequired<DominatorSet>();
	}
	virtual bool runOnFunction(Function &F);

    protected:
	void DuplicaAllBB(Function &F, std::list<ChildrenSet*> *HeadNodeList);
	void DupOnIFs(std::list<ChildrenSet*> *); 
	bool inEdgesMarked(ChildrenSet*node, std::set<Edge*>&MarkedEdge);

	virtual void IFDupforNode(ChildrenSet*){}; //do not implement
	
	void markMidNodes(std::set<BasicBlock*>&, std::set<ChildrenSet*>*);
	ChildrenSet* OneofPattern(BasicBlock *, std::list<ChildrenSet*> *);

	void specialDupBB(BasicBlock *, ChildrenSet*);
	Instruction *findPosin(ChildrenSet*);
	BasicBlock* preRepBlock(BasicBlock *, Instruction*);
	BasicBlock *RepBrBlock (BasicBlock *,Instruction*, BasicBlock*);
	bool ImplementonEdge(Edge*, unsigned int, Instruction*);
	bool noEffect(Instruction*);
	void replaceLocalOperands(Instruction *, std::map<Value*,Value*>&);

	std::map<BasicBlock*,BasicBlock*> BBupdateMap;

    }; //end of class IfDuplica

    

    class ParIfDuplica : public IfDuplica {
    private:
	virtual void IFDupforNode(ChildrenSet*);
    };

    class FullIfDuplica : public IfDuplica{
    private:
	virtual void IFDupforNode(ChildrenSet*);
    };

}//end of namespace

#endif //IFDUPLICA_H
