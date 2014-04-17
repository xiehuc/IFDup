//-----Developed by Jing Yu--------------//
//   ParIFDuplica.cpp                  //
//=======================================//
//Partially duplicate comparisons in IFs     //
//=======================================//

#define Jing_DEBUG 1

#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "llvm/DerivedTypes.h"

#include "ShortcutConstruct.h"
#include <list>

using namespace llvm;

namespace {
    class ParIFDuplica : public FunctionPass{ 
	virtual bool runOnFunction(Function &F);
	virtual void getAnalysisUsage(AnalysisUsage &AU) const {
	    AU.addRequired<DominatorSet>();
	}

        private:
	void IFDupPar(std::list<ChildrenSet*>*);
	void DupImplement(std::list<ChildrenSet*>*);
	bool inEdgesMarked(ChildrenSet *, std::set<Edge*>&);
	void IFDupforNode(ChildrenSet *);
	Instruction *findPosin(ChildrenSet *);
	bool ImplementonEdge(Edge *,unsigned int,std::map<Value*,Value*>&,Instruction*);
	BasicBlock* RepBlock(BasicBlock*,std::map<Value*,Value*>&,Instruction*,BasicBlock *);
	void preRepBlock(BasicBlock*,std::map<Value*,Value*>&,Instruction*);
	bool noEffect(Instruction*);
	void replaceOperands(Instruction *,std::map<Value*,Value*>&,std::map<Value*,Value*>&);
	void setbranchtoError(BranchInst *, unsigned int);
        BasicBlock * buildErrorBlock(Function&);
	void UpdateIncomeSource(BasicBlock *,BasicBlock *,BasicBlock*);
	int localnumreplicatedBB;
        bool canbecopied(Instruction*);
	BasicBlock *errorBlock;

        public:
	void DEBUG_outputsethead(ChildrenSet*, std::set<ChildrenSet*>*);
    }; //end of functionpass

    static  Statistic<> NumReplicatedBB("numreplicatedBB", "Number of replicated BBs");

}

//register to OPT pass
namespace {
 RegisterOpt<ParIFDuplica> X("ParIFDup", "Partially Duplicate IF");
}



//////////////////////////////
//ParIFDuplica Class      ///
//////////////////////////////
bool ParIFDuplica::runOnFunction(Function &F) {

    localnumreplicatedBB = 0;
    //get DominatorSet
    DominatorSet &dominset_use = getAnalysis<DominatorSet>();

    //get SCHeadNodeList
    ShortcutDetectorPass *SCDetectorPass = new ShortcutDetectorPass();
    std::list<ChildrenSet*> *HeadNodeList = SCDetectorPass->runOnFunction(F,dominset_use);

    if (!HeadNodeList->empty()) {
	//Partially duplicate IF
	IFDupPar(HeadNodeList);

	//we can check what's going on on the edges
	SCDetectorPass->dumpShortcut(*HeadNodeList);

	//well then, implement the duplicas in LLVM code
	errorBlock = buildErrorBlock(F);
#ifdef Jing_DEBUG
	//std::cerr << "errorBlock was set to be " << errorBlock->getName() <<"\n";
#endif
	DupImplement(HeadNodeList);

#ifdef Jing_DEBUG_EXIT
   //==========================test exit===========
  std::string exitName = "exit";
  std::vector<const Type*> ArgTys;
  ArgTys.push_back(Type::IntTy);
  FunctionType *exitType = FunctionType::get(Type::VoidTy, ArgTys,false);
 Function *callfun = F.getParent()->getFunction(exitName,exitType);
 BasicBlock *firstB = &F.front();
 std::cerr << "firstB on F is " << firstB->getName() <<"\n";
 ConstantSInt *const_3 = ConstantSInt::get(Type::IntTy,-23);
 std::string fakecallName = "";
 Instruction *lastI = firstB->getTerminator();
  Instruction * fakenewCall = new CallInst(callfun,const_3,fakecallName,lastI);
  ///////=========end of test exit===========
#endif

  std::cerr << "local replicated BB: " << localnumreplicatedBB<<"\n\n";
    } 
#ifdef Jing_DEBUG
     else { std::cerr << "no change was made to " << F.getName()<<"\n";}
#endif

    return true;
}

BasicBlock *ParIFDuplica::buildErrorBlock(Function &F) {
  //the error block is inserted at the end
  BasicBlock *EB = new BasicBlock(F.getName()+"_Error",&F); 

 //insert the void exit(int) function to the module
  std::string exitName = "exit";
  std::vector<const Type*> ArgTys;
  ArgTys.push_back(Type::IntTy);
  FunctionType *exitType = FunctionType::get(Type::VoidTy, ArgTys,false);
  Function *callfun = F.getParent()->getFunction(exitName,exitType);
 
  if (callfun) {
#ifdef Jing_DEBUG
      //std::cerr << "we find exit func\n";
#endif
  }
 else {
#ifdef Jing_DEBUG
     //std::cerr << "exit() is not found. let's insert one\n";
#endif
     callfun = F.getParent()->getOrInsertFunction(exitName,exitType);
     assert(callfun && "Failed exit() declaration insertion");
 }

  //add call Inst to EB
  ConstantSInt *const_23 = ConstantSInt::get(Type::IntTy,-23);
  std::string callName = "";
  Instruction * newCall = new CallInst(callfun,const_23,callName,EB);

  /* 
  //still has to place a dummy ret there
  const Type *myrettype = F.getFunctionType()->getReturnType();
  Constant *retValue = Constant::getNullValue(myrettype);
  Instruction *dummy_ret = new ReturnInst(retValue,EB);  
  */

  Instruction * endinst = new UnreachableInst(EB);
  return EB;
}

void ParIFDuplica::IFDupPar(std::list<ChildrenSet*> *HeadNodeList){
    std::list<ChildrenSet*>::iterator iter;
    ChildrenSet *SCHead;
    for (iter=HeadNodeList->begin(); iter!=HeadNodeList->end(); iter++) {
	SCHead=*iter;
	std::set<ChildrenSet*> *midnodeset = SCHead->getSCmidnodeset();
	std::list<ChildrenSet*> WorkList;
	std::set<Edge*> MarkedEdge;
	WorkList.push_front(SCHead);
#ifdef Jing_DEBUG
	//DEBUG_outputsethead(SCHead, midnodeset);
#endif
	//use this variable to verify if every midnode was touched
	unsigned int worked_number=0;

	while (!WorkList.empty()) {
	    ChildrenSet *curNode = WorkList.front();
	    WorkList.pop_front();
	    worked_number++;

#ifdef Jing_DEBUG
	    //std::cerr << "IFDupPar workon "<< curNode->getBB()->getName() <<"\n";
#endif
	    //work on curNode
	    //This is the function that complicated duplication algorithm should modify
	    IFDupforNode(curNode);

	    //marked out0 and out1
	    MarkedEdge.insert(curNode->out0);
	    MarkedEdge.insert(curNode->out1);

	    //if target of out0 is within midnodeset &&
	    //all its incoming edges are marked, insert that node to WorkList
	    if (ChildrenSet *leftchild=curNode->leftchildrenset) {
		if ((midnodeset->count(leftchild)>0) && (inEdgesMarked(leftchild,MarkedEdge)))
		    WorkList.push_back(leftchild);
	    }

	    //if target of out1 is within midnodeset &&
	    //all its incoming edges are marked, insert that node to WorkList
	    if (ChildrenSet *rightchild = curNode->rightchildrenset) {
		if ((midnodeset->count(rightchild)>0) && (inEdgesMarked(rightchild,MarkedEdge))) {
		    WorkList.push_back(rightchild);
		}
	    }
		    
	} //end of while
	
	//check if every midnode has been touched
	assert((worked_number==(midnodeset->size()+1)) && "Error:not all midnodes were touched!");
    } //end of for
}



//check if node's incoming edges have an entry in MarkedEdge
//This function is called when one incoming edge of this node was just accessed.
bool ParIFDuplica::inEdgesMarked(ChildrenSet *node, std::set<Edge*>& MarkedEdge) {
    //If this node has only one incoming edge, it means all its incoming edge has been accessed.
    if (node->inEdges->size()<=1) return true;
    std::list<Edge*>::iterator iter;

    for (iter = node->inEdges->begin(); iter != node->inEdges->end(); iter++) {
	Edge * inedge = *iter;
	if (MarkedEdge.count(inedge) == 0) return false;
    }
    return true;
}


//this is partial propagation algorithm
void ParIFDuplica::IFDupforNode(ChildrenSet *curNode) {

    assert(curNode->out0->isnoRep() && "Error: the edge should not have Reps");
    assert(curNode->out1->isnoRep() && "Error: the edge should not have Reps");

    if (curNode->inEdges) {
	////////deal with incoming edges////////////////
	//if there are more than one incoming edge
	//fix their Reps. We will not propagate them. - may be optimized later - Jing
	if (curNode->inEdges->size()>1) {
	    std::list<Edge*>::iterator iter,iterend;
	    for (iter=curNode->inEdges->begin(),iterend=curNode->inEdges->end(); iter!=iterend; iter++) 
		(*iter)->fixAllReps();
	}
	//if there is only one incoming edge
	//propagate its Reps
	else {
	    Edge *inEdge = curNode->inEdges->front();
	    
	    while (!inEdge->isPRepEmpty()) {
		//thie popFirstRep() will pop out the first element
		Rep *curRep = inEdge->popFirstRep();
		//propagateTo() will check if curRep is allowed to be prpagated to that node
		curNode->out0->propagateTo(curRep);
		curNode->out1->propagateTo(curRep);
	    }
	}
    }
    /////////////////////////////////////////////////
    //replicate myself and propagate to outgoing edges
    if (!curNode->haveSC) {
    Rep *replica0 = new Rep(curNode->getBB(),true);
    curNode->out0->insertRep(replica0);
    Rep *replica1 = new Rep(curNode->getBB(),false);
    curNode->out1->insertRep(replica1);
    } else {
	BasicBlock *leftB = curNode->out0->getTo();
	BasicBlock *rightB = curNode->out1->getTo();

	if (curNode->isleftSC) {
	    Rep *replica0 = new Rep(curNode->getBB(), true);
	    curNode->out0->insertRep(replica0);
	    Rep *replica1 = new Rep(curNode->getBB(),false,leftB);
	    curNode->out1->propagateTo(replica1);
	} else {
	    Rep *replica0 = new Rep(curNode->getBB(),true, rightB);
	    curNode->out0->propagateTo(replica0);
	    Rep *replica1 = new Rep(curNode->getBB(), false);
	    curNode->out1->insertRep(replica1);
	}
    }
}


//for debug purpose
void ParIFDuplica::DEBUG_outputsethead(ChildrenSet *SCHead, std::set<ChildrenSet*> *midnodeset){
    std::cerr << "===output sethead for " << SCHead->getBB()->getName() <<"===\n";
    std::set<ChildrenSet*>::iterator iter;
    for (iter=midnodeset->begin(); iter!=midnodeset->end(); iter++) {
	ChildrenSet * midNode = *iter;
	std::cerr << " " << midNode->getBB()->getName();
    }
    std::cerr<< "\n";
}


///////////////////////////////////////////////
//DupImplement.h is to implement this method
//void ParIFDuplica::DupImplement(std::list<ChildrenSet*> *HeadNodeList){
///////////////////////////////////////////////
#include "DupImplement.h"



/////////////////////////////////////
//Ugly!!! The below file can stand alone, but
//for some silly linking problem, I have to have
//it inlined here.
///////////////////////////////////////
#include "ShortcutConstruct.cpp.in"
