//////////////////////////////////////
//  SafeRegOPT.h                    //
//////////////////////////////////////
// Provide data structure for advanced register safe optimization. //
// The data structure will mainly record the safe values for each  //
// basic block.													   //
/////////////////////////////////////////////////////////////////////

#ifndef SAFEREGOPT_H
#define SAFEREGOPT_H

#include <llvm/IR/Value.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Support/CFG.h>
#include <llvm/IR/Function.h>
#include <llvm/ADT/Statistic.h>

	
#include <map>
#include <set>
#include <vector>
#include <string>

using namespace llvm;

namespace llvm {

//////////////////////
// Tool functions   //
//////////////////////
class tool{
public:
static void copySet(std::set<Value*>* cur, std::set<Value*>&result) {
	   result.clear();
	   if (cur->empty()) return;
	   for (std::set<Value*>::iterator it = cur->begin(), ie = cur->end();
	   	    it != ie; it++) {
		 result.insert(*it);	
	   }
	}

static	void intersect(std::set<Value*>* cur, std::set<Value*>&result) {
	   if (cur->empty()) {
		 result.clear();
		 return;
	   }
	   std::set<Value*> *temp = new std::set<Value*>();
	   for (std::set<Value*>::iterator it = result.begin(),
	   								   ie = result.end();	it != ie; it++) {
		 if (cur->find(*it) != cur->end()) {
			temp->insert(*it);
	     }
	   }
	   copySet(temp, result);
	   delete temp;
	}
};

//////////////////////////
// class SafeRegforBB   //
//////////////////////////
// This class records two sets of safe values for each BB: at the begining
// and at the end.
class SafeRegforBB {
  private:
    int curIndex_;
  public:
	int numInComing;  // the number of incoming edges
	//Pointers to safe value sets of all incoming edges
	std::vector<std::set<Value*>*> inComingEdges;
	std::set<Value*> safeRegSet;

	std::set<Value*> *getSafeRegSet() { return &safeRegSet;};

	void setIncoming(int edge, std::set<Value*>* in) {
	  assert(edge < numInComing && "Edge exceeds the number of incoming blocks.");
	  assert(inComingEdges[edge] == NULL && "inComingEdges already has an entry set for this edge");
	  inComingEdges[edge] = in;
	}

	// If this block does not contain PHI node, we don't care the order
	// of the imcoming edges.
	void pushIncoming(std::set<Value*>* in) {
	  setIncoming(curIndex_, in);
	  curIndex_++;
	}

	bool isValueSafeonIncoming(int edge, Value* v) {
	  assert(edge < numInComing && "Edge exceeds the number of incoming blocks.");
	  if (inComingEdges[edge] != NULL) {
	    if (inComingEdges[edge]->count(v) > 0)
		  return true;
	  }

	  return false;
	}

    bool isValueSafe(Value *v) {
	 if (safeRegSet.find(v) == safeRegSet.end()) return false;
	 else return true;
	}

	void insertValueSafe(Value *v) {
	  assert(!isValueSafe(v) && "Value v is already in safe set");
	  safeRegSet.insert(v);
	}

	// Calculate intersection of all available inComingEdges.
	// If inComingEdge is NULL, means that block has not beed processed yet.
	// Based on our processing order, that edge must be a backward edge.
    void computeSafeRegSet() {
	  if (numInComing > 0) {
	    int i = 0;
	    std::set<Value*> *cur;
		bool first = true;

	    while (i < numInComing) {
		  cur = inComingEdges[i];
		  if (cur != NULL) {
			if (first) {
			  // Copy the first unnull set to safeRegSet.
			  tool::copySet(cur, safeRegSet);
			  first = false;
			} else {
			  // Intersect. Put result in safeRegSet.
			  tool::intersect(cur, safeRegSet);
			}
		  }
		  i++;
        }
	  }
	}

   // Print the safeRegSet set
   void dumpSafeRegs() {
     std::cerr << "(";
	 std::set<Value*>::iterator i, e;
	 for (i=safeRegSet.begin(), e=safeRegSet.end(); i!=e; i++) {
		std::cerr << (*i)->getName() << " ";
	 }
	std::cerr <<")\n";
   }

   // Constructor and destructor.
	SafeRegforBB(int edges) {
	  safeRegSet.clear();
 	  numInComing = edges;
	  curIndex_ = 0;

	  //Initialize inComingEdges.
	  inComingEdges.clear();
	  for (int i = 0; i < edges; i++) {
		inComingEdges.push_back(NULL);
	  }
	}

	~SafeRegforBB() {
	  safeRegSet.clear();
	}
};

//////////////////////////
// class SafeRegMap     //
//////////////////////////
// This class has a map mapping from every BB to its SafeRegforBB structure.
// This class also provides interface for InsDuplica.cpp to manipulate
// the SafeRegforBB structure.
class SafeRegMap {
  public:
    std::map<BasicBlock*, SafeRegforBB*> safeRegMap;

	SafeRegforBB* getSafeRegsforBB(BasicBlock *BB) {
		assert(safeRegMap.find(BB) != safeRegMap.end() && 
		       "Can not find the BB entry");
		return safeRegMap[BB];
	}


	// Constructor and destructor
	SafeRegMap(Function &F) {
	  for (Function::iterator BBi = F.begin(), BBE = F.end();
	       BBi != BBE; ++BBi) {
		 BasicBlock *BB = BBi;

		 // Count how many predecessors
		 int count = 0;
		 pred_iterator PI = pred_begin(BB), E = pred_end(BB);
		 while (PI != E) {
			++PI;
			++count;
		 }
		 SafeRegforBB *safeRegs = new SafeRegforBB(count);
		 safeRegMap[BB] = safeRegs;
	  }
	}

	~SafeRegMap() {
	  // Drop SafeRegforBB objects.
	  for (std::map<BasicBlock*, SafeRegforBB*>::iterator
	       it = safeRegMap.begin(), ie = safeRegMap.end();
		   it != ie; it++) {
		SafeRegforBB *safeRegs = (*it).second;
		if (safeRegs != NULL) delete safeRegs;
	  }
	}
};

} // end of namespace

#endif
