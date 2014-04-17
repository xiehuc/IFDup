//--Developed by Jing Yu--------------//
//ShortcutDetector.h                 //
//====================================//

#ifndef SHORTCUTDETECTOR_H
#define SHORTCUTDETECTOR_H

#include "llvm/Pass.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Function.h"
#include "llvm/Instructions.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Analysis/Dominators.h"
//#include "llvm/Support/Debug.h"

#include <set>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <list>


using namespace llvm;

namespace {

    class ChildrenSet;

    class Rep {
    public:
	Rep(BasicBlock *baseBB, bool trueside) {
	    mybaseBB = baseBB;
	    isOntrueside = trueside;
	    mynotTo = NULL;
	}

	Rep(BasicBlock *baseBB, bool trueside, BasicBlock* notTo) {
	    mybaseBB = baseBB;
	    isOntrueside = trueside;
	    mynotTo = notTo;
	}
	/* --ugly. hope it will not be used
	Rep(Rep &duplica) {
	    mybaseBB = duplica.getBB();
	    isOntrueside = duplica.getOntrueside();
	    mynotTo = duplica.getnotTo();
	}
	*/
    private:
	BasicBlock *mybaseBB; //BB to be replicated
	bool isOntrueside; //if this replica is on true side
	BasicBlock *mynotTo; //not propagate to the edge that has notTo as destination

    public:
	bool getOntrueside() {return isOntrueside;}
	BasicBlock *getnotTo(){return mynotTo;}
	BasicBlock *getBB() {return mybaseBB;}

        bool notTo(BasicBlock *target) {return (mynotTo == target);}
	bool notTo(ChildrenSet *target);

	std::string dump() {
	    std::string s;
	    s+="Rep("+mybaseBB->getName()+",";
	    s+=(isOntrueside?"T,":"F,");
	    if (mynotTo) s+="Not("+mynotTo->getName()+")";
	    return s;
	}
    }; //end of struct Rep


    ///////////////////////////////////////
    ///////////////////////////////////////   
 class Edge {
 public:
     Edge(ChildrenSet *from, BasicBlock *to) {fromNode = from; toNode = to;}
     Edge(ChildrenSet *from, ChildrenSet *to);
    private:
      ChildrenSet *fromNode;
      BasicBlock *toNode;
      std::list<Rep*> propgtRep;
      std::list<Rep*> fixRep;

    public:
      Rep* popFirstRep() { 
	  Rep *firstone = propgtRep.front();
	  propgtRep.pop_front();
	  return firstone;
      }
     void insertRep(Rep *one) { //insert to fixRep
	 fixRep.push_back(one);
     }
     void propagateTo(Rep *one) { //propagate to propgtRep
	 if (!one->notTo(toNode)) { //this edge can be propagated to
	     propgtRep.push_back(one);
	 }
     }
     bool isPRepEmpty() {return propgtRep.empty();}
     bool isFRepEmpty() {return fixRep.empty();}

     std::list<Rep*> getfinalRep() {
	 assert(propgtRep.empty() && "getfinalRep must be called after all propagation is done!");
	 fixRep.unique();
	 return fixRep;
     }

     std::string dump(std::string prefix);

     std::string dump(std::list<Rep*> &listtodump) {
	 if (listtodump.empty()) return " ";

	 std::string *s = new std::string();
	 std::list<Rep*>::iterator iter;
	 for (iter=listtodump.begin(); iter!=listtodump.end(); iter++) {
	     (*s)+=(*iter)->dump()+" ";
	 }
	 return (*s);
     }

    };//end of struct Edge

 /////////////////////////////////
 ////////////////////////////////
 //struct DominatorSet; 
 Statistic<> NumShortcut("numshortcut", "Number of shortcut branches detected");
 Statistic<> NumShortcutSet("numshortcutset", "Number of shortcut branche SETs detected");


  class ChildrenSet {
  public:
     ChildrenSet(BasicBlock *thisBB, BasicBlock *leftleaf, BasicBlock*rightleaf);
     ChildrenSet(BasicBlock *thisBB, BasicBlock *leftleaf, ChildrenSet *rightSet);
     ChildrenSet(BasicBlock *thisBB, ChildrenSet *leftSet, BasicBlock *rightleaf);
     ChildrenSet(BasicBlock *thisBB, ChildrenSet *leftSet, ChildrenSet *rightSet);
    inline ~ChildrenSet();

    private:
      BasicBlock *myBB;
      //std::set<BasicBlock*> allchnodes;  --in old version
      std::set<ChildrenSet*> *SCmidnodeset;
      std::list<bool> *mySCpath;
      int level;
      bool head; 
      int nummidnodes;
      int SCnum;
      void init(BasicBlock *thisBB, int leftlevel, int rightlevel);
      ChildrenSet *uplink;
      bool isMomsLchild;

      //union two children's allchnodes into current children nocdes
      //void setUnion(ChildrenSet *childset1, ChildrenSet *childset2);
      //void setUnion(ChildrenSet*childset1, BasicBlock *chlid2);
      //void setUnion(BasicBlock *child1, BasicBlock *child2);
      //bool isShortcut(BasicBlock *child1, ChildrenSet *childset2);
      ChildrenSet * isShortcut(BasicBlock *child1, ChildrenSet *childset2, int *lastLeft);
      std::string dump(std::string, std::set<ChildrenSet*>*,ChildrenSet*);

      
    public:
      ChildrenSet *leftchildrenset, *rightchildrenset;
      BasicBlock *leftchildBB, *rightchildBB;
      bool haveSC, isleftSC, isrightSC;
      bool isHead() {return head;}
      void invalidateHead() {head = false;}
      int getLevel() {return level;}
      int getSCnum() {return SCnum;}
      //std::set<BasicBlock*> *getCHnodes() {return &allchnodes;}
      std::set<ChildrenSet*> *getSCmidnodeset() {return SCmidnodeset;}
      BasicBlock *getBB() {return myBB;}
      void setUplink(ChildrenSet *up,bool isLchild ) {uplink = up; isMomsLchild = isLchild; }
      ChildrenSet * getUplink() {return uplink;}
      bool isMomLchild() {return isMomsLchild;}
      std::set<ChildrenSet*> *getallmidnodeset(ChildrenSet* findMidnode,ChildrenSet* pathstart, int* totalSCnum);
      std::list<bool> *getmySCpath(ChildrenSet* findMidnode, ChildrenSet* pathstart, int lastLeft, int* mynummidnodes);     
      bool verify_domination(DominatorSet *dominset);
      void dump();


      void ChildrenSetUnion(std::set<ChildrenSet*>*,std::set<ChildrenSet*>*);
      void ChildrenSetInsert(std::set<ChildrenSet*>*, ChildrenSet*);

  public:
      Edge *out0, *out1;
      std::list<Edge>* inEdges;
      void conEdgeGraph(std::set<ChildrenSet*>*);
      void addinEdges(Edge &);
 };

    //////////////////////////////////////
    //////////////////////////////////////
 class ShortcutDetectorPass : public FunctionPass {
    virtual bool runOnFunction(Function &F);

    virtual void getAnalysisUsage(AnalysisUsage &AU) const{
	AU.addRequired<DominatorSet>();
        AU.setPreservesAll();
    }

   private:
     bool isTwowayBranch (BasicBlock *BB);
     bool isOnlyBranch (BasicBlock *BB);
     bool isJumpBack (BasicBlock *BB, BasicBlock *Target);
     void dumpShortcut (std::list<ChildrenSet*> &headlist);
     bool hasBackEdge(BasicBlock *BB);
     int localshortcut, localSCset, localFailed;
     DominatorSet *dominset;
     void conSCSetMap(std::set<BasicBlock*>&, std::set<BasicBlock*>& ,std::map<BasicBlock*,ChildrenSet*>&);
     void BuildHeadNodeList (std::map<BasicBlock*,ChildrenSet*>&, Function &);
     void ClearUselessNodesin (std::map<BasicBlock*,ChildrenSet*>&, std::list<ChildrenSet*>&); 
     bool verify_domination(ChildrenSet *);
     std::list<ChildrenSet*> HeadNodeList;
     void conEdgeGraph(std::list<ChildrenSet*>&);
    
 public:
 std::list<ChildrenSet*> getHeadNodeList() {return HeadNodeList;}

 };

}


#endif
