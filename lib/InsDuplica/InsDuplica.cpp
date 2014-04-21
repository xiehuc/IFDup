//-----Developed by Jing Yu -------------//
// InsDuplica.cpp                        //
//=======================================//
//Duplicate all instructions             //
//future work: implement GSR             //
//=======================================//
//Has redundant checks analysis.
//Option: Register safe

//Protect br with redundant branches.

//#define FUNC_DEBUG 1

#define DEBUG_TYPE "ins_duplica"
#define REG_SAFE 1

#include <set>
#include "RedundOPT.h"

#include "InsDuplica.h"

//#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/PostOrderIterator.h"

//#define Jing_DEBUG 1

STATISTIC(NumInsDup, "Number of generated instructions");
STATISTIC(NumBBChecker, "Number of generated branch checker BBs");
STATISTIC(NumStoreChecker, "Number of generated store checker BBs");      

using namespace llvm;

namespace {
    RegisterPass<InsDuplica> X("InsDup", "Duplicate all Instructions");
    RegisterPass<InsDuplicaTile> Y("InsDupTile", "Duplicate all Instructions in tile");
}

//////////////////////////////
//Implement InsDuplica Class//
//////////////////////////////

bool InsDuplica::runOnFunction(Function &F) {
    //initiate local counters
    initLocalCounter();

    //if the function is not dummy, we need to work on it
    if (notdummyFunc(F) && workFunc(F)) {

	//get postDominatorSet for redundant check removal
	PostDominatorTree &postDominTree = getAnalysis<PostDominatorTree>();
	LoopInfo &loopinfo = getAnalysis<LoopInfo>();

#ifdef REG_SAFE
	DominatorTree &DominTree  = getAnalysis<DominatorTree>();
	domintree = &DominTree;
#endif

	mycheckCodeMap = new CheckCodeMap();
	myvalueCheckedAtMap = new ValueCheckedAtMap();

#ifdef REG_SAFE
    // Initialize a map to recording safe registers
	safeRegMap = new SafeRegMap(F);
#endif

	//apply redundant analysis
	RedundAnalysis redundAnalysisPass;
	redundAnalysisPass.SetUpTable(mycheckCodeMap, myvalueCheckedAtMap, F);

#ifdef REG_SAFE
    redundAnalysisPass.checkADVRegSafe(dominset);
#endif
//	redundAnalysisPass.removeOverlap(mycheckCodeMap,myvalueCheckedAtMap,F,postDominSet);
//	redundAnalysisPass.rmLoopIV(mycheckCodeMap,myvalueCheckedAtMap,F,loopinfo);

#ifdef REG_SAFE
	//this pass is only for stat purpose. This only counts the checks for
	// loaded values.
	// We don't need this pass any more. We will count checks here.
	//redundAnalysisPass.rmSafeReg(mycheckCodeMap,myvalueCheckedAtMap,F);
#endif

	//build the error-exit BB
	 errorBlock = buildErrorBlock(F);

	DuplicaAllBB (F);

	//dump stat for checks
	redundAnalysisPass.printStatforTotal(F);

	//delete redundant analysis tables
	delete mycheckCodeMap;
	delete myvalueCheckedAtMap;

#ifdef REG_SAFE
	delete safeRegMap;
#endif
    }
    
    //dump local counters
    counterdump(F);
    

    return true;
}


/////////////////////////////////////
///DuplicaAllBB()                  //
/////////////////////////////////////
void InsDuplica::DuplicaAllBB(Function &F) {

    valueMap.clear();
    toAddvalueMap.clear();

    std::list<BasicBlock*> WorkList;
    std::set<BasicBlock*> markedBB;

    WorkList.clear();
    markedBB.clear();

    //error block should not be duplicated
    markedBB.insert(errorBlock); 
 
    //add all BBs to WorkList.
#ifdef REG_SAFE
    // Get the topological ordered tree.
    ReversePostOrderTraversal<Function*> PROT(&F);
    for (ReversePostOrderTraversal<Function*>::rpo_iterator
	     I = PROT.begin(), E = PROT.end(); I != E; ++I) {
       BasicBlock *BB = *I;
	   WorkList.push_back(BB);
	}
#else
	// We don't need to care the order.
    for (Function::iterator BBi = F.begin(), BBE = F.end(); BBi!=BBE; ++BBi) {
      BasicBlock *BB = BBi;
	  WorkList.push_back(BB);
    }
#endif

    //while working list is empty. Do.
    while (!(WorkList.empty())) {
	  BasicBlock *curBB = WorkList.front();
	  WorkList.pop_front();

#ifdef Jing_DEBUG
	    std::cerr << "\n---Duplicate curBB: " << curBB->getName() <<" ---\n";
#endif
	  //if curBB has not been replicated, let's replicate it
	  if (markedBB.count(curBB) == 0) {
#ifdef REG_SAFE
#ifdef Jing_DEBUG
		/* This part is only valid when DuplicaBB() is not called, because
		// the pred link will change after replication and checking.
	    //
		// We need to make sure the imcoming blocks for curBB have been
		// processed, except backward edges.
		pred_iterator PI = pred_begin(curBB), E = pred_end(curBB);
		while (PI != E) {
		  BasicBlock *PB = *PI;
		  if (markedBB.count(PB)==0) {
			// PB->curBB must be a backward edge. Or PB is non-reachable.
			if (!(dominset->dominates(curBB, PB))) {
			  if (dominset->isReachable(PB)) {
			    if (PI != pred_begin(curBB)) {
				  BasicBlock *PF = *(pred_begin(curBB));
				  //if the first pred dominates curBB, it is fine.
				  if (!(dominset->dominates(PF, curBB))) {
				    std::cerr << "Block(" << PB->getName() << 
			               ") has not been processed\n";
			        assert(0 && "order fail");
				  }
				}
			  }
			}
		  }
		  ++PI;
		}
	   */
#endif
#endif
	    // mark curBB
	    markedBB.insert(curBB);

#ifdef REG_SAFE
		// Point curBB's childern's incoming blocks.
		SafeRegforBB* saferegs = safeRegMap->getSafeRegsforBB(curBB);
		std::set<Value*> *safeSet = saferegs->getSafeRegSet();
		BranchInst *term = dyn_cast<BranchInst>(curBB->getTerminator());
        if (term) {
		  int numS = term->getNumSuccessors();
		  for ( int i = 0; i < numS; i++ ) {
			BasicBlock *suc = term->getSuccessor(i);
			if (markedBB.count(suc) == 0) {
			  SafeRegforBB *safeSucc = safeRegMap->getSafeRegsforBB(suc);
			  // If suc has PHI, we will make sure incoming safe reg set
			  // is inserted at correct place.
			  if (PHINode *phI = dyn_cast<PHINode>(suc->begin())) {
				int index = phI->getBasicBlockIndex(curBB);
				safeSucc->setIncoming(index, safeSet);
			  } else {
				safeSucc->pushIncoming(safeSet);
			  }
			}
		  }
		}
#endif
 	    //replicate curBB
	    DuplicaBB(curBB);
	  }
    }
    assert(toAddvalueMap.empty() && "toAddvalueMap must be empty now");
}

////////////////////////////////////
///DuplicaBB()                    //
////////////////////////////////////
void InsDuplica::DuplicaBB(BasicBlock *BB) {

#ifdef REG_SAFE
    // Get safe reg sets for current BB
    curSafeRegs = safeRegMap->getSafeRegsforBB(BB);
    // Calculate intersection from incoming safe reg sets
 	curSafeRegs->computeSafeRegSet();
#ifdef Jing_DEBUG
    std::cerr << "||IN||curSafeRegs:";
	curSafeRegs->dumpSafeRegs();
#endif
#endif

    Instruction *nextI;
    //find last condition or terminator instruction
    Instruction *LastCond = findLastCond(BB);
    Instruction *I = BB->begin();

    //process argument on fitst BB
    Function *Fun = BB->getParent();
    if (BB == Fun->begin()) {
	dupFuncArgu(*Fun);
    }

    //replicate instructions inside BB
    while (I!=LastCond && I!=NULL) {
	nextI = I->getNext();
	//duplicate I and insert the duplicated instruction before I
	if (duplicable(I)) {
		//this version, we use load-move version for load
		if (LoadInst *loadI = dyn_cast<LoadInst>(I)) {
			BB = DuplicaLoad(loadI,BB);
		} else 
	    	DuplicaInst(I,I);
	} else{
	    if (isSynchPoint(I)) {
		//if I is synchpoint, check correcness before proceed
		//this may add new blocks.
		BB=newCheckerSynch(I,BB,nextI);
	    };

	    valueMap[I]=I;
	}
	I=nextI;
    }

    //if I is null, means this block has ended
    if (I != NULL) { 
	//check branch
	//If terminator of BB is a conditional branch
	if (BranchInst *BI = hasConditionalBr(BB)) {
	    DuplicaBr(BB, LastCond, BI);
	} else if (dyn_cast<ReturnInst>(LastCond)) 
	    newCheckerSynch(LastCond, BB, I);      
    }
#ifdef Jing_DEBUG
    std::cerr << "||OUT||curSafeRegs:";
	curSafeRegs->dumpSafeRegs();
#endif

}




/////////////////////////////////////
///DuplicaBBtile()                 //
/////////////////////////////////////
void InsDuplicaTile::DuplicaBB(BasicBlock *BB) {
    //find last condition or terminator instruction
    Instruction *LastCond = findLastCond(BB);
    Instruction *I = BB->begin();
    
    Instruction *nextSynI;
    Instruction *zoneEnd;
    Instruction *nextI;

    //tile instructions
    while (I!=LastCond) {
	nextI=I->getNext();
	//deal with synchPoints
	while ((I!=LastCond) && (isSynchPoint(I))){
	    //if I is synchpoint, check correcness before proceed
	    BB=newCheckerSynch(I,BB,nextI);
	    valueMap[I]=I;
	    I = nextI;
	}
	if (I==LastCond) break;

	//Now I is at the beginning of a new synch zone
	//Find the end of this zone
	nextSynI = findNextSynchPoint(I,LastCond);
	zoneEnd = nextSynI->getPrev();
	Instruction *lastI;

	do {
	    if (duplicable(I)){
		//PHI node must be grouped at top of basic block!
		if (isa<PHINode>(I)) DuplicaInst(I,I);
		else DuplicaInst(I,nextSynI);
	    } else {
		valueMap[I]=I;
	    }
	    lastI = I;
	    I=I->getNext();
	} while(lastI!=zoneEnd);

	//skip those new generated instructions
	I=nextSynI;
    }
    
    //check branch
    //If terminator of BB is a conditional branch
    if (BranchInst *BI = hasConditionalBr(BB)) {
	DuplicaBr(BB, LastCond, BI);  
    }
}

//////////////////////////////////////
//newCheckerSynch()                //
/////////////////////////////////////
BasicBlock* InsDuplica::newCheckerSynch(Instruction* synchI, BasicBlock *BB, Instruction * &nextI) {
    assert ((synchI->getParent()) ==BB && "newCheckerSynch: synchI's parent must be BB");
    
    //If synchI is store, add checker block to comapre its value and pointers.
    StoreInst * StoreI = dyn_cast<StoreInst>(synchI);
    if (StoreI) {
	return newCheckerStore(synchI, BB, nextI);
    } 

    //Remove redundant checks
    if (isa<ReturnInst>(synchI) || isa<CallInst>(synchI)) {
	BasicBlock *newBB = BB;
	std::set<Value*> *tocheck = mycheckCodeMap->getCheckElemList(synchI);
	if (tocheck && !tocheck->empty()) {
	    
	    localnumfinalothercheck += tocheck->size();
	    std::string nametag;
	    if (isa<ReturnInst>(synchI)) nametag = "rV";
	    else if (isa<CallInst>(synchI)) nametag = "cV";
	    
	    for (std::set<Value*>::iterator ii = tocheck->begin(), e=tocheck->end(); ii!=e; ii++) 
		newBB = newOneValueChecker((*ii), synchI, newBB, nametag);	    
	}
	return newBB;
    }
    
    return BB;
}

///////////////////////////////////
//newCheckerStore()           //
///////////////////////////////////
BasicBlock *InsDuplica::newCheckerStore(Instruction* synchI, BasicBlock *BB, Instruction * &nextI) {
    StoreInst * StoreI = cast<StoreInst>(synchI);
    //get the address
	Value *addrP = StoreI->getPointerOperand();
	//Instruction *addrPI = dyn_cast<Instruction>(addrP);
	//get the value to be stored
	Value *StoreValue = StoreI->getOperand(0);
	//Instruction *StoreValueI = dyn_cast<Instruction>(StoreValue);

	BasicBlock *BBtobeSpliced = BB;
	BasicBlock *newBB=BB;

	//Remove redundant checks
	std::set<Value*> *tocheck = mycheckCodeMap->getCheckElemList(StoreI);
	if (tocheck && !tocheck->empty()) {
	    assert(tocheck->size() <=3 && "Do not allow to check too many checks" );
	    localnumfinalstcheck += tocheck->size();

	    std::string nametag = "A";
	    for (std::set<Value*>::iterator ii = tocheck->begin(), e=tocheck->end(); ii!=e; ii++) 
		newBB = newOneValueChecker((*ii), StoreI, newBB, nametag);	    
	}

	return newBB;  
}


///////////////////////////////////
//newOneValueChecker()           //
///////////////////////////////////
BasicBlock *InsDuplica::newOneValueChecker(Value *ValuetoCheck, Instruction *synchI, BasicBlock *BBofSynchI, std::string &nameTag) {
    assert (duplicable(ValuetoCheck) && "checked value must be duplicable");

#ifdef REG_SAFE
    //Register Safe  SafeReg
    //if ValuetoCheck is a load or cast(load), do not check
    /*  -- We are generalizing the rule.
	if (isa<LoadInst>(ValuetoCheck)) return BBofSynchI;
    if (CastInst *castI = dyn_cast<CastInst>(ValuetoCheck)) {
      if (isa<LoadInst>(castI->getOperand(0)))
        return BBofSynchI;
    }
    */
	if (curSafeRegs->isValueSafe(ValuetoCheck)) {
	  // Count the checks for reg safe
	  statRegRemove(synchI);
	  return BBofSynchI;
	}
	if (CastInst *castI = dyn_cast<CastInst>(ValuetoCheck)) {
	  if (curSafeRegs->isValueSafe(castI->getOperand(0))) {
		// Count the checks for reg safe
		statRegRemove(synchI);
		return BBofSynchI;
	  }
	}
#endif

    Value *ValuetoCheckDup = ValuetoCheck; //by default

    //If ValuetoCheck's dup is itself. Do not check it.
    if (valueMap.count(ValuetoCheck) >0) 
	if (valueMap[ValuetoCheck] == ValuetoCheck) 
	  return BBofSynchI;

    //new SetEQ instruction and insert it before synchI
    SetCondInst *newSetEQ = new SetCondInst(llvm::Instruction::SetEQ, ValuetoCheck, ValuetoCheckDup, ValuetoCheck->getName()+nameTag, synchI);
    newSetEQ->setDUP(); //set DUP attribute
    localnuminsdup++;
    NumInsDup++;

#ifdef Jing_DEBUG
    std::cerr << "create newSetEQ for " <<ValuetoCheck->getName() <<" at " << BBofSynchI->getName()<<"\n";
#endif
    //update ValuetoCheckDup
    if (valueMap.count(ValuetoCheck) > 0) {
	ValuetoCheckDup = valueMap[ValuetoCheck];
	newSetEQ->setOperand(1,ValuetoCheckDup);
    } else { 
	//StoreValue should have a duplica. 
	//Since we haven't found it, sumbit a request an update request
	assert(isa<Instruction>(ValuetoCheck) && "Argu must be already in valueMap");
	requestToMap(cast<Instruction>(ValuetoCheck), newSetEQ);
    }

    //split BBofSynchI to add conditional branch
    BasicBlock *newBB = BBofSynchI->splitBasicBlock(synchI, BBofSynchI->getName()+nameTag);

    //Now, the end of BBofSynchI is a branch to newBB. We have to replace this branch by a conditional branch based on newSetEQ
    BranchInst *BI = dyn_cast<BranchInst>(BBofSynchI->getTerminator());
    assert(BI && "After split, the splitted BB must have a Br as its terminator");
    //Erase the old branch
    BI->eraseFromParent();
    BranchInst *condBI = new BranchInst(newBB,errorBlock,newSetEQ,BBofSynchI);
    condBI->setDUP(); //set DUP attribute

    localnuminsdup++;
    NumInsDup++;
    
    //no need to update PhINodes. splitBasicBlock already does so

#ifdef Jing_DEBUG
    std::cerr << "newOneValueChecker creates "<<newBB->getName()<<" inside "<< BBofSynchI->getName()<<"\n";
#endif

#ifdef REG_SAFE
    // Now the value is safe.
	curSafeRegs->insertValueSafe(ValuetoCheck);
#ifdef Jing_DEBUG
	std::cerr << "add_safe("<<ValuetoCheck->getName()<<"),";
#endif
#endif
    localnumStorechecker++;
    NumStoreChecker++;
    return newBB;
}

////////////////////////////////
// statRegRemove              //
////////////////////////////////
// Count the number of checks that can be removed because of reg safe.
void InsDuplica::statRegRemove(Instruction *I) {
#ifdef Jing_DEBUG
  std::cerr << "save 1,";
#endif
  if (isa<LoadInst>(I)) localnumadvregcheckld ++;
  else if (isa<StoreInst>(I)) localnumadvregcheckst ++;
  else if (isa<BranchInst>(I)) localnumadvregcheckbr ++;
  else localnumadvregcheckother ++;
}

///////////////////////////////////
//DuplicaBr()                   //
///////////////////////////////////
// Cureent version checks the comparion by doing comparison again. Replace
// operands.
void InsDuplica::DuplicaBr(BasicBlock *BB, Instruction *LastCond, BranchInst *BI) {
    assert(BI->isConditional() && "Only conditional branch will be replicated");
    assert((LastCond->getParent())==BB && "LastCond's parent must be BB");

    //But condition could be "call". So be careful that this instruction can not be replicated

    Instruction *myCond = dyn_cast<Instruction>(BI->getCondition());
    assert (myCond && "Branch condition must not be trivial");
    
    /* -- below is to check the correctness of comparison operands --
    //Remove redundant checks
    //std::set<Value*> *tocheck = mycheckCodeMap->getCheckElemList(BI);
    

   
    BasicBlock *newBB = BB;
    bool checkOperand = false;

   //still duplicate condition. In the case, the condition has one more uses, 
   //we have the duplicant.   
   if (LastCond != BI) {
        assert(myCond->getParent() == BB && "if LastCond!=BI, then myCond must be inside BB");
        assert(myCond==LastCond && "if LastCond!=BI, then LastCond = myCond");
        assert(myCond->getNext()==BI && "if LastCond!=BI, then myCond must be in front of BI");

	//if myCond is setXX, check its operands
	if (dyn_cast<SetCondInst>(myCond)) { 
	    checkOperand = true;
	    std::string nametag = "bV";

	    //Remove redundant checks - get elements from tocheck set
	    if (tocheck && !tocheck->empty()) {
		localnumfinalbrcheck += tocheck->size();

		for (std::set<Value*>::iterator i = tocheck->begin(), e=tocheck->end(); i!=e; i++) 
		    newBB = newOneValueChecker((*i), LastCond, newBB, nametag);
	    }

	} //end of if <SetCondInst>

	//If myCond is setXX and it has only one use, we clone the setXX
	//in new checker block. Otherwise, we dup it inside current BB.
        if (!(isa<SetCondInst>(myCond))) {
	    if (duplicable(LastCond)) DuplicaInst(LastCond,LastCond);
            else {
                valueMap[LastCond] = LastCond;
	    }
	} else { //setXX and has multiple uses. we dup it but does not 
	    //relace its operands
	    if (!(myCond->hasOneUse())) {
		Instruction *newI = myCond->clone();
		newI->setDUP();
		if (myCond->hasName()) {
		    newI->setName(newI->getName() + "_dup");
		}
		myCond->getParent()->getInstList().insert(myCond,newI);
		valueMap[myCond]=newI;
		updateUsersMap(myCond,newI);

		NumInsDup++;
		localnuminsdup++;		
	    }
	}


   } else {
       //if LastCond ==BI, condition of BI must be outside BB or PHI
       //or myCond has more than one use
       assert((isa<PHINode>(myCond)||(myCond->getParent() != BB) || !(myCond->hasOneUse())) && "if LastCond==BI, then myCond must be outside BB or PHI or multiple uses");
   }


   //If the condition's operand is not checked, we need to check the bool value
   if (!checkOperand) {
       //remove redundant checks
       if (tocheck && !tocheck->empty()) {
	   if (duplicable(myCond)) {
	       std::string nametag = "bV";
	       newBB = newOneValueChecker(myCond, BI, newBB, nametag);

	       localnumfinalbrcheck ++;
	   }
       }
   }
  
   */

   // current version: Just replicate the comparison and replace operands.
   //                  Some conditions can not be replicated.

   BasicBlock *newBB = BB;
   if (LastCond != BI) {
     // Condition of BI must be inside BB and the condition has only
     // single use.
     assert(myCond->getParent() == BB && 
            "if LastCond!=BI, then myCond must be inside BB");
     assert(myCond==LastCond && "if LastCond!=BI, then LastCond = myCond");
     assert(myCond->getNext()==BI &&
            "if LastCond!=BI, then myCond must be in front of BI");

     // Make sure LastCond has only one use - sometimes not true, in which
     // case, we must duplicate it inside BB.
     if ((isa<PHINode>(LastCond)) || !(LastCond->hasOneUse())) {
       if (duplicable(LastCond))
	   //duplicate condition at where it is.
	   DuplicaInst(LastCond, LastCond);
       else
	   valueMap[LastCond] = LastCond;
     }
   } else {
       // If LastCond == BI, condition of BI must be outside BB or PHI
       //or myCond has more than one use.
       assert((isa<PHINode>(myCond) || (myCond->getParent() != BB) ||
	       !(myCond->hasOneUse())) &&
	      "If LastCond == BI, then myCond must be outside BB or PHI or multiple uses");
   }


   //add two new blocks to check the conditional branch itself
   BasicBlock *trueTarg = BI->getSuccessor(0);
   BasicBlock *falseTarg = BI->getSuccessor(1);


   //make new checker BB and insert them before BI's original successors.
   BasicBlock *newtrueTarg = newCheckerBB(LastCond,BI,newBB,trueTarg,true);
   BasicBlock *newfalseTarg = newCheckerBB(LastCond,BI,newBB,falseTarg,false);

   //link BB to new targets
   BI->setSuccessor(0, newtrueTarg);
   BI->setSuccessor(1, newfalseTarg);

   NumBBChecker += 2;
   localnumBBchecker +=2;   

  
   //Remove Redundant checks - optimize on loop IV and invariants
   if (CheckCode *checkcodeforbr = mycheckCodeMap->getCheckCode(BI)) {
       CheckBranch *checkbr = (CheckBranch*)checkcodeforbr;
       unsigned int propsize = checkbr->PropCheckSize();
       if (checkbr > 0) {
	   //insert value checking to the beginning of newtrueTarg or newfalseTarg
	   std::string dummy="";
	   for (unsigned int is = 0; is < propsize; is++) {
	       Value *valuetocheck = checkbr->getPropCheckValue(is);
	       bool propto = checkbr->getPropTo(is);
	       BasicBlock *synBB = newfalseTarg;
	       if (propto) {
		   synBB = newtrueTarg;
	       }
	       Instruction *synII = synBB->begin();
	       std::ostringstream isstream;
	       isstream << is;
	       std::string numstring = dummy+isstream.str()+"pV";
	       newOneValueChecker(valuetocheck, synII, synBB, numstring);
	   }
       } //end of if >0
   }//end of  checkcode
}


///////////////////////////////////
//newBBinsert()                  //
///////////////////////////////////
BasicBlock* InsDuplica::newCheckerBB(Instruction *LastCond, BranchInst *BI, BasicBlock *thisBB,BasicBlock *nextBB, bool trueSide) {
    std::string sidetag = (trueSide)? "T":"F";
    std::string newName = thisBB->getName()+"_"+sidetag+"_"+nextBB->getName();
    
    BasicBlock *newBB = new BasicBlock(newName, thisBB->getParent(), nextBB);
    
    Instruction *myCond = dyn_cast<Instruction>(BI->getCondition());
    assert(myCond && "BI's condition must not be trivial");

    Value *newCond = myCond; //by default

    if (LastCond==BI) {
	assert((isa<PHINode>(myCond) || myCond->getParent() != thisBB || !(myCond->hasOneUse())) && "if LastCond==BI, then myCond must be outside thisBB or PHI" );
	if (valueMap.count(myCond) > 0)
	    newCond = valueMap[myCond];
	
    } else {
	//Here some assertions are not true, because the original BB has been splited by checks
	//assert(myCond->getParent() == thisBB && "if LastCond!=BI, then myCond must be inside BB");
	assert(myCond==LastCond && "if LastCond!=BI, then LastCond = myCond");
	//assert(myCond->getNext()==BI && "if LastCond!=BI, then myCond must be in front of BI");

	if (isa<SetCondInst>(myCond)) {
	    if (!(myCond->hasOneUse())) {
		assert(valueMap.count(myCond)>0 && "setXX must have been replicated");
		newCond = valueMap[myCond];
	    } else {
		//dup setXX in new BB
		//Note that LastCond won't have an entry in valueMap, because it has two duplica.
		Instruction *newCondI = LastCond->clone();
                newCondI->setDUP(); //set DUP attribute
		if (LastCond->hasName()) 
		    newCondI->setName(LastCond->getName()+"_"+sidetag+"dup");
		replaceOperands(newCondI);  // replace operands
		newBB->getInstList().push_back(newCondI);
		newCond = newCondI;
		NumInsDup++;
		localnuminsdup++;		
	    }

	}

    }

    //duplicate branch
    BranchInst *newBI;
    if (trueSide) 
	newBI = new BranchInst(nextBB,errorBlock,newCond,newBB);
    else
	newBI = new BranchInst(errorBlock,nextBB,newCond,newBB);

    newBI->setDUP(); // set DUP attribute


    //if myCond is outside BB, and myCond has not been replicaed, we have to
    //submit a update request
    if ((myCond != LastCond) && (newCond == myCond)) {
	if (duplicable(myCond))
	    requestToMap(myCond,newBI);
    }


    //modify incoming sources in PhiNodes of nextBB
    updatePHInodesBB(nextBB, thisBB, newBB);
    

    NumInsDup++;
    localnuminsdup++;

    return newBB;
}



///////////////////////////////////
//DuplicaInst()                 //
//////////////////////////////////
void InsDuplica::DuplicaInst(Instruction *I, Instruction *insertBefore) {

    assert(duplicable(I) && "I must be duplicable");
  
    Instruction *newI = I->clone();
   
    newI->setDUP(); //this is a duplicated instruction
    if (I->hasName()) {
	newI->setName(I->getName() + "_dup");
    }
    //replicate its operands
    replaceOperands(newI);
    //update valueMap
    valueMap[I]=newI;
    //update its users
    updateUsersMap(I,newI);

    I->getParent()->getInstList().insert(insertBefore,newI);
    NumInsDup++;
    localnuminsdup++;
}

////////////////////////////////
//DuplicaLoad()               //
///////////////////////////////
BasicBlock* InsDuplica::DuplicaLoad(LoadInst *I, BasicBlock *BB) {
	assert(I->getParent()==BB && "I's parent must be BB");
	BasicBlock *newBB = BB;

	/*check I's address
	Value *addrP = I->getOperand(0);
	if (duplicable(addrP)) {
	  std::string nametag = "lA";
	  newBB = newOneValueChecker(addrP, I, BB, nametag);
	}
	*/

	//Remove redundant checks
	std::set<Value*> *tocheck = mycheckCodeMap->getCheckElemList(I);
	if (tocheck && !tocheck->empty()) {

	    localnumfinalldcheck += tocheck->size();

	    assert(tocheck->size() <=2 && "Do not allow to check too many checks" );
	    std::string nametag = "lA";
	    for (std::set<Value*>::iterator ii = tocheck->begin(), e=tocheck->end(); ii!=e; ii++) 
		newBB = newOneValueChecker((*ii), I, newBB, nametag);	    
	}

#ifdef REG_SAFE
	valueMap[I] = I;
	updateUsersMap(I,I);
	curSafeRegs->insertValueSafe(I);
#else
	// Comment for Register Safe
	//dup I - backend generator will convert this load to a move
	Instruction *newI = I->clone();
	newI->setName(I->getName() + "_dup");
	I->getParent()->getInstList().insert(I->getNext(),newI);
	valueMap[I]=newI;
	updateUsersMap(I,newI);
    //
#endif

	NumInsDup++;
	localnuminsdup++;
	


	return newBB;
}

/////////////////////////////////
//replaceOperands()            //
/////////////////////////////////
 void InsDuplica::replaceOperands(Instruction *newI) {
     unsigned int numOP = newI->getNumOperands();
	 int stride = 1;

	 // for PHINode, values are on i*2 positions.
	 if (isa<PHINode>(newI)) stride = 2;

     for (unsigned int i=0; i<numOP; i+=stride) {
	 Value *curOP = newI->getOperand(i);

#ifdef REG_SAFE
       // If curOP is on safe reg set, we will not replace it.
	   if (curSafeRegs->isValueSafe(curOP)) {
#ifdef Jing_DEBUG
		  std::cerr << "safe(";
		  if (newI->hasName()) std::cerr << newI->getName() <<":";
		  else std::cerr << "U:";
		  
		  std::cerr << curOP->getName() <<"),";
#endif
		  continue;
	   }
	   
	   // For a PHINode, we will check the corresponding incoming edge.
	   if (isa<PHINode>(newI)) {
		 // The index for this incoming block is i/2
		 if (curSafeRegs->isValueSafeonIncoming(i/2, curOP)) {
#ifdef Jing_DEBUG
			std::cerr <<"PHIsafe(I:"<<i/2<<",v:"<<curOP->getName()<<"),";
#endif
		 }
		 continue;
	   }
#endif

	 if (valueMap.count(curOP) > 0) {
	     //curOP has a replica (or dummy replica)
	     if (valueMap[curOP] != curOP) 
		 newI->setOperand(i,valueMap[curOP]);
	 } else {
	     //currently curOP does not have an entry
	     //we check if this curOP is duplicable, if yes then we insert a update request to toAddvalueMap
	     if (Instruction *curOPI = dyn_cast<Instruction>(curOP)) {
		 //if curOP is not an instruction(const or BasicBlock*), we can not duplicate it
#ifdef Jing_DEBUG
		 std::cerr << "curOP does not have a copy " << curOP->getName() << " at " << newI->getName() << "\n";
#endif
		 requestToMap(curOPI, newI);
	     }
	 } //end of if
     } //end of for
 }


 /////////////////////////////////////
 //requestToMap()                   //
 /////////////////////////////////////
 void InsDuplica::requestToMap(Instruction *curOPI, Instruction *newI) {
     if (duplicable(curOPI)) {
#ifdef Jing_DEBUG
	 std::cerr << "submit update request for " << curOPI->getName() << " at " << newI->getName() <<"\n";
#endif
	//submit an update request of curOPI to toAddvalueMap
	 std::list<Instruction*> * requestList;
	 if (!(toAddvalueMap.count(curOPI))) {
	     requestList = new std::list<Instruction*>;
	     requestList->clear();
	     toAddvalueMap[curOPI]=requestList;
	 } else {
	     requestList = toAddvalueMap[curOPI];
	 }
	 requestList->push_back(newI);
     }
 }


 ////////////////////////////////////////////
 //updateUsersMap()                         //
 //Lookup I on toAddvalueMap, if found a    //
 //request from requestor, replace I with   //
 //newI in requestor's operands list        //
 /////////////////////////////////////////////
 void InsDuplica::updateUsersMap(Instruction *I, Instruction *newI) {
     if (toAddvalueMap.count(I)) {
	 std::list<Instruction*> *requestList = toAddvalueMap[I];
	 for (std::list<Instruction*>::iterator iter=requestList->begin(), iend=requestList->end(); iter!=iend; ++iter) {
	     Instruction *requester = (*iter);
#ifdef Jing_DEBUG
	     std::cerr << "update "<< I->getName() <<" List. requester="<<requester->getName()<<"\n";
#endif
	     unsigned int numOP = requester->getNumOperands();
	     bool find = false;
	     for (unsigned int i=0; i<numOP; i++) {
		 if ((requester->getOperand(i)) == I) {
		     find = true;
		     requester->setOperand(i,newI);
		     break; //find one an then break;
		 }
	     }
	     assert(find && "You must have found I in requester's operands list");
	 }
	 //remove this entry from toAddvalueMap
	 requestList->clear();
	 toAddvalueMap.erase(I);
     }
 }


//////////////////////////////////////
//duplicable()                  
//Ins that could not be duplicated inside this BB:
//  --synchPoint
//  --alloca, malloca
//  --va_arg
// -- volatile load
//////////////////////////////////////
bool InsDuplica::duplicable (Value *V) {
    if (Instruction *Ins = dyn_cast<Instruction>(V)) {
      if (isSynchPoint(Ins) || isa<AllocationInst>(Ins) ||  isa<VAArgInst>(Ins))  return false;
	} else {
	  return isFuncArgu(V);
	}
/***now we just load once. so it is safe.
    if (LoadInst *ldIns = dyn_cast<LoadInst>(Ins)) 
	if (ldIns->isVolatile()) return false;
*/
    return true;
}

///////////////////////////////////////
//findNextSynchPoint()
//Ins must be a non-synchpoint of BB.
//Func: find the next SynchPoint instruction
//This can be used to tile instrutions inside BB
////////////////////////////////////////
Instruction *InsDuplicaTile::findNextSynchPoint (Instruction *curI, Instruction*lastCond) {
    assert(!(isSynchPoint(curI)) && "findNextSynchPoint: curI must not be a Synchpoint");
    while (curI!=lastCond && !(isSynchPoint(curI))) 
	curI = curI->getNext();
    return curI;
}

//////////////////////////////////////
//isSynchPoint()
//Test if this instruction is SynchPoint:
//  -- call, rewind, invoke
//  -- store
//  -- terminator
bool InsDuplica::isSynchPoint (Instruction *Ins) {
    if (isa<CallInst>(Ins) || isa<TerminatorInst>(Ins) || isa<StoreInst>(Ins) ||isa<FreeInst>(Ins) ) return true;
    return false;
}

//////////////////////////////////////
//isBranchCond()
//Test if this instruction is the last setCC or boolean instruction that provides value to the conditional branch(switch)
//////////////////////////////////////
bool InsDuplica::isBranchCond ( Instruction *Ins) {
    TerminatorInst * lastIns = Ins->getParent()->getTerminator();
    assert(!(isa<SwitchInst>(lastIns)) && "Find a SwitchInst! You need to lower SwitchInst.");
    if (BranchInst* BI = dyn_cast<BranchInst>(lastIns)) 
	if (BI->isConditional()) {
	    Instruction* Cond = dyn_cast<Instruction>(BI->getCondition());
	    if (Cond && Cond == Ins) return true;
	}
    return false;
}


////////////////////////////////////
//hasConditionalBr()              //
////////////////////////////////////
BranchInst *InsDuplica::hasConditionalBr(BasicBlock *BB) {
    TerminatorInst *terminator = BB->getTerminator();
    //if terminator is a branch
    if (BranchInst *BI = dyn_cast<BranchInst>(terminator)) {
	//if it is conditional branch, we will make new blocks to check the branch
	if (BI->isConditional()) return BI;
    }
    return NULL;
}


/////////////////////////////////////
//findLastSyn()                    //
//Find the last instruction in BB. Any instructions before this one must be replicated except Synchpoint
/////////////////////////////////////
Instruction* InsDuplica::findLastCond (BasicBlock *BB) {
    TerminatorInst *lastIns = BB->getTerminator();
    assert(!(isa<SwitchInst>(lastIns)) && "Find a SwitchInst! You need to lower SwitchInst.");
    BranchInst * BI = dyn_cast<BranchInst>(lastIns);
    if (BI && (BI->isConditional())) {
	Value *cond = BI->getCondition();
	Instruction* condIns = dyn_cast<Instruction>(cond); 
	//find condIns. But we have to make sure condIns is the second to the last instruction in BB
	assert(condIns && "Branch Condition must not be trivial");
	if ((condIns->getNext())!=lastIns) {
	    //assert((condIns->getParent() == BB) && "condIns is not in the same BB as br!");
	    //if condIns is not in the same BB as br. We have to leave it there
	    if (condIns->getParent() != BB) return lastIns;
	    //if condIns is a PHINode, since we can not move, we just return BI
	    //if condIns has more than one use, better not to reorder
	    if ((isa<PHINode>(condIns))||!(condIns->hasOneUse())) return lastIns;
	    condIns->moveBefore(lastIns); //we moved condInst right before br
#ifdef Jing_DEBUG
	    std::cerr << "adjust order of condIns "<< condIns->getName() <<" in " << BB->getName() <<"\n";
#endif
	}
#ifdef Jing_DEBUG
    std::cerr << "findLastCond returns " << condIns->getName() <<"\n";
#endif
    return condIns;
    
    }
    //return the terminator instruction
    return lastIns; 
}


///////////////////////////////////
//notdummyFunc()                 //
///////////////////////////////////
bool InsDuplica::notdummyFunc(Function &F) {
    //currently, if the function has only one BB and this BB has only one instrution, we will not work on it
    if (++(F.begin()) == F.end()) {
	BasicBlock *BB = F.begin();
	if (++(BB->begin()) == BB->end()) 	return false;
    }
    return true;
}

////////////////////////////////////
//workFunc()                      //
////////////////////////////////////
bool InsDuplica::workFunc(Function &F) {
#ifdef FUNC_DEBUG
   std::set<std::string> notWorkingFunc;
//   notWorkingFunc.insert("CollectGarb");
//   notWorkingFunc.insert("ProdInt");
//   notWorkingFunc.insert("EvVar");
//   notWorkingFunc.insert("NewBag");
//   notWorkingFunc.insert("SumInt");
   
   std::string funcname = F.getName();
   if (notWorkingFunc.find(funcname) == notWorkingFunc.end()) {
     //func name is not within notWorkingFunc
     return true;
   } else
   return false;
#else
  return true;
#endif  //FUNC_DEBUG
}


////////////////////////////////////
//buildErrorBlock                 //
////////////////////////////////////
BasicBlock *InsDuplica::buildErrorBlock(Function &F) {
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
     //std::cerr << "exit(int) is not found. let's insert one\n";
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
  //Now we have UnreachableInst as the terminator! neat
  Instruction * endinst = new UnreachableInst(EB);
  return EB;
}


////////////////////////////
//updatePHInodesBB()      //
////////////////////////////
void InsDuplica::updatePHInodesBB(BasicBlock *myBB, BasicBlock *fromBB, BasicBlock *toBB) {
    for (BasicBlock::iterator I=myBB->begin(), E=myBB->end(); I!=E; ++I) {
	if (PHINode *PhI = dyn_cast<PHINode>(I)) {
	    int IDX = PhI->getBasicBlockIndex(fromBB);
	    while (IDX != -1) {
		PhI->setIncomingBlock(IDX, toBB);
#ifdef Jing_DEBUG
		std::cerr<< "replace PHInode("<<PhI->getName()<<")'s income from "<<fromBB->getName() <<" to " << toBB->getName() <<"\n";
#endif
		IDX = PhI->getBasicBlockIndex(fromBB);
	    }
	}
    }

}

/////////////////////////
//dupFuncArgu()        //
/////////////////////////
void InsDuplica::dupFuncArgu(Function &F) {
  BasicBlock *firstBB = F.begin();
  Instruction *firstI = firstBB->begin();
  for (Function::arg_iterator AI = F.arg_begin(), E = F.arg_end();
           AI != E; ++AI) {
	 arguSet.insert(AI);

	 //if argument is used only once or all on the same block,
	 //do not dup it.
	 if (AI->hasOneUse()) {
	   valueMap[AI] = AI;
	 } else {
 	 	//make copy of cast
	  	const Type *arguType = AI->getType();
	 	CastInst *newCast = new CastInst(AI, arguType, AI->getName()+"_dup", firstI);
	 	valueMap[AI] = newCast;
     }
 }
}

///////////////////////////
//isFuncArgu()           //
///////////////////////////
bool InsDuplica::isFuncArgu(Value *v) {
	if (arguSet.find(v) != arguSet.end()) return true;
	else return false;
}

////////////////////////////
//counterdump()           //
////////////////////////////
void InsDuplica::counterdump(Function&F) {
    std::cerr << "local generated branch checker BBs: " << localnumBBchecker <<" ("<< F.getName() <<")\n";
    std::cerr << "local generated store checker BBs: " << localnumStorechecker <<" ("<<F.getName() <<")\n";
    std::cerr << "local generated instructions: " << localnuminsdup << " (" << F.getName() <<")\n";

    //for redundant checkings

   std::cerr << "LOCAL_REDUND_CHECK "<< localnumfinalldcheck - localnumadvregcheckld <<" localnumfinalldcheck ("<<F.getName()<<")\n";
    std::cerr << "LOCAL_REDUND_CHECK "<< localnumfinalstcheck - localnumadvregcheckst <<" localnumfinalstcheck ("<<F.getName()<<")\n";
     std::cerr << "LOCAL_REDUND_CHECK "<< localnumfinalbrcheck - localnumadvregcheckbr <<" localnumfinalbrcheck ("<<F.getName()<<")\n";
     std::cerr << "LOCAL_REDUND_CHECK "<< localnumfinalothercheck - localnumadvregcheckother <<" localnumfinalothercheck ("<<F.getName()<<")\n";

//REG_SAFE
	// for advanced register safe
   std::cerr << "LOCAL_REDUND_CHECK "<< localnumadvregcheckld <<" localnumadvregcheckld ("<<F.getName()<<")\n";
    std::cerr << "LOCAL_REDUND_CHECK "<< localnumadvregcheckst <<" localnumadvregcheckst ("<<F.getName()<<")\n";
     std::cerr << "LOCAL_REDUND_CHECK "<< localnumadvregcheckbr <<" localnumadvregcheckbr ("<<F.getName()<<")\n";
     std::cerr << "LOCAL_REDUND_CHECK "<< localnumadvregcheckother <<" localnumadvregcheckother ("<<F.getName()<<")\n";


}

////////////////////////////
//initLocalCounter()      //
////////////////////////////
void InsDuplica::initLocalCounter() {
    localnuminsdup = 0;
    localnumStorechecker=0;
    localnumBBchecker = 0;


    //for redundant checkings
    localnumfinalldcheck = 0;
    localnumfinalstcheck = 0;
    localnumfinalbrcheck = 0;
    localnumfinalothercheck = 0;

// REG_SAFE
    // for advanced register safe
	localnumadvregcheckld = 0;
	localnumadvregcheckst = 0;
	localnumadvregcheckbr = 0;
	localnumadvregcheckother = 0;

}


