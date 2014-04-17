//---Developed by Jing Yu ---- //
//LdAddrAnalysis.h            //
//============================//
#ifndef LDADDRANALYSIS_H
#define LDADDRANALYSIS_H

#include "llvm/Pass.h"
#include "llvm/Instruction.h"
#include "llvm/Instructions.h"
#include "llvm/Function.h"
#include "llvm/ADT/Statistic.h"

#include <map>
#include <string>
#include <list>
#include <iostream>
#include <set>

using namespace llvm;

namespace llvm {
 struct InstPair{
     private:
        Instruction *origI;
	Instruction *dupI;
     public:
	InstPair(Instruction *o, Instruction*d) {origI = o; dupI=d;}
	void add(Instruction *I, bool isdup) {
	    if (isdup) {
		//debug
		/*
		if (dupI) {
		  std::cerr << "dupI occupied.I="<<I->getName()<<" " << dump();
		}
		*/

		assert(!dupI && "dupI occupied. Exam same name instruction.");
		dupI = I;
	    } else {
		//debug
		/*
		if (origI) {
		    std::cerr << "origI occupied. I="<<I->getName()<<" " <<dump();
		}
		*/
		assert (!origI && "origI occupied. Exam same name instruction.");
		origI = I;
	    }
	}
      InstPair(Instruction *I, bool isdup) {
	    origI = NULL;
	    dupI = NULL;
	    add (I, isdup);
      }
     bool isPair() {
	 if (origI && dupI) return true;
	 else return false;
     }
     bool hasdupentry() {
	 if (dupI) return true;
	 else return false;
     }

     Instruction *getdupI() {
	 return dupI;
     }
     std::string dump() {
	std::string temp = "Ipair(origI=" ;
	if (origI) temp+= origI->getName() + " ,dupI=";
	else temp += "null,dupI=";
	if (dupI) temp+=dupI->getName()+")";
	else temp+="null)";
	if (isPair()) temp += "P";
	return temp;
     } 
 }; //end of struct InstPair

 class InstDupMap {
    private:
	std::map<std::string, InstPair*> InstMap;
	int classifydup(Instruction *);
	std::string &getkey(Instruction *I);
    public:
	InstDupMap();
	~InstDupMap();
	bool isdup(Instruction *I);
	bool hasTF(Instruction*);
	void buildMap(Instruction *I);
	bool hasDup(Instruction *I);
	void clear();
 }; //end of class InstDupMap


 class LdAddr : public FunctionPass {
    public:
	virtual bool runOnFunction(Function &F);
    private:
	bool isSynchPoint(Instruction*);
	bool duplicable (Instruction *);
      	bool workingInstType(Instruction *);
	InstDupMap instmap;
	void setupmap(Function &F);
	std::list<LoadInst*> loadlist;
	void scanLoadList();
	void scanCondset();
	void scanLoad(LoadInst *);
	Value * scanCast(Value *);
	int scanValue(Value*,int);
        void needprotect(Instruction *, bool);

	int scanCondOperandValue(Value *operand, Instruction *condI);

	bool isOriginalBranch(BranchInst *bI, std::string &errorblockname);
 	std::set<Instruction*> needprotectset;
        std::set<Instruction*> needprotectIV;
	std::set<Instruction*> condset;
        
	int localtotalload;
	int localprotectedload;
	int localunprotectedload;
	int localindphi;
	int localloadaddr;

	int localbrconduseload;
	int localnumnotsafecondition;
	int localnumcondnodup;
	int localtotalcond;
	int localbrcondduetoiv;
	int localbrcondduetold;
	int localbrcondduetoldiv;
	

 }; //end of class LdAddr

} //end of namespace llvm



#endif
