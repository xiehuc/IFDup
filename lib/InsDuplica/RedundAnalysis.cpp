////////////////////////////////////////
//RedundAnalysis.cpp                  //
////////////////////////////////////////
//Analyze reduncy of checking codes.  //
//Set up CheckCodeMap,                //
//     and ValueCheckedMap.           //
//Optimization will be done on other  //
//objects.                            //
////////////////////////////////////////
//Option: L1, L3(L2)
//        Check or do not check br operands.
/////////////////////////////////////////

#include <llvm/ADT/SmallPtrSet.h>
#include "llvm/PostDominators.h"
#include "RedundOPT.h"

#define L1_CHECK 1
//#define L3_CHECK 1

//#define CHECK_BRANCH_OPERANDS 1
#define DECOMPOSE_ADDR 1

#ifdef L1_CHECK
#undef L3_CHECK
#else
#define L3_CHECK 1
#endif

#define R_DEBUG 1

////////////////////////////
//Class CheckCode         //
////////////////////////////
void
CheckCode::insertOrigElement(Value *elem) {
    if (CheckElem.find(elem) == CheckElem.end()) {
	origNumElem ++;
	CheckElem.insert(elem);
	assert(CheckElem.size() == origNumElem && "CheckElem is corrupted");
    }
}

void
CheckCode::deleteElement(Value *elem) {
    std::set<Value*>::iterator i = CheckElem.find(elem);
    if (i == CheckElem.end()){
	assert(0 && "Can not find elem in CheckElem");
    } else {
     CheckElem.erase(i);
    }
}

void
CheckCode::dumpCheckCode() {
	errs() << "I(" ;
	if (CheckCodeInst->hasName()) errs()<<CheckCodeInst->getName();
	else errs() << CheckCodeInst->getParent()->getName();
	errs() <<")-T("<< CheckCodeInst->getOpcodeName()<<")-[";
	for (std::set<Value*>::iterator i=CheckElem.begin(),e=CheckElem.end(); i != e; i++) 
		errs() << (*i)->getName() <<" ";
	errs()<<"]";
}

void
CheckCode::dump() {
	dumpCheckCode();
	errs() <<"\n";
}

////////////////////////////
//Class CheckLoad         //
////////////////////////////
CheckLoad::CheckLoad(Instruction *I)
    : CheckCode(I) {
    computeFinal = false;
    finalElems.clear();
}

CheckLoad::~CheckLoad() {
    CheckElem.clear();
    finalElems.clear();
    computeFinal = false;
}

unsigned int
CheckLoad::getOrigNumElem() {
    //TODO later
    return (origNumElem>0?1:0);
    //return origNumElem;
}

std::set<Value*>& 
CheckLoad::getCheckElemList(){
    //TODO later
    if (!computeFinal) ComputeFinal();
    return finalElems;
    //turn CheckElem;
}

unsigned int
CheckLoad::getFinalNumElem() {
    //TODO later - return 1 if FinalElem!=null;
    if (!computeFinal) ComputeFinal();
    return finalElems.size();
    //return CheckElem.size();
}
void 
CheckLoad::ComputeFinal() {
    //compute finalElems out of CheckElem
    assert(!computeFinal && "this func has already been called");
    if (CheckElem.size() >0) {
	Value *addrP = CheckCodeInst->getOperand(0);
	//addrP not in CheckElem. ok. do not check anything
	if (CheckElem.find(addrP) != CheckElem.end()) {
	    //see what are other options
	    if (CheckElem.size()-1 >1) {
		//after decompose, we still need to check at least 2.
		//then we just check the original addrP
		finalElems.insert(addrP);
	    } else {
		//we check decomposed addresses
		for (std::set<Value*>::iterator i=CheckElem.begin(), e=CheckElem.end(); i!=e; i++) {
		    if ((*i) != addrP) {
			finalElems.insert(*i);
		    }
		}
	    }
	} 
    }
    computeFinal = true;
}


void
CheckLoad::dump() {
	dumpCheckCode();
	errs() <<"-LoadFinal(";
	if (computeFinal) {
		for (std::set<Value*>::iterator i = finalElems.begin(), e=finalElems.end(); i!=e; i++) {
			errs() << (*i)->getName() <<" ";
		}
	}
	errs() << ")\n";
}


////////////////////////////
//Class CheckStore        //
////////////////////////////
CheckStore::CheckStore(Instruction *I)
    : CheckCode(I) {
    computeFinal = false;
    finalElems.clear();
}

CheckStore::~CheckStore() {
    CheckElem.clear();
    finalElems.clear();
    computeFinal = false;
}

unsigned int
CheckStore::getOrigNumElem() {
    // later
    if (origNumElem<=2) return origNumElem;
    else return 2;
    //return origNumElem;
}

std::set<Value*>& 
CheckStore::getCheckElemList(){
    //later
    if (!computeFinal) ComputeFinal();
    return finalElems;
    //turn CheckElem;
}

unsigned int
CheckStore::getFinalNumElem() {
    // - return 1 if FinalElem!=null;
    if (!computeFinal) ComputeFinal();
    return finalElems.size();
    //return CheckElem.size();
}


void 
CheckStore::ComputeFinal() {
    //compute finalElems out of CheckElem
    assert(!computeFinal && "this func has already been called");
    int valuecheck = 0;
    if (CheckElem.size() >0) {
	Value *valueP = CheckCodeInst->getOperand(0);
	if (CheckElem.find(valueP) != CheckElem.end()) {
	    finalElems.insert(valueP);
	    valuecheck = 1;
	}
	if (CheckElem.size() - valuecheck > 0) {
	    Value *addrP = CheckCodeInst->getOperand(1);
	    //addrP not in CheckElem. ok. do not check anything
	    if (CheckElem.find(addrP) != CheckElem.end()) {
		//see what are other options
		if ((CheckElem.size()-1-valuecheck) >1) {
		    //after decompose, we still need to check at least 2.
		    //then we just check the original addrP
		    finalElems.insert(addrP);
		} else {
		    //we check decomposed addresses
		    for (std::set<Value*>::iterator i=CheckElem.begin(), e=CheckElem.end(); i!=e; i++) {
			if ((*i) != addrP && (*i) != valueP) {
			    finalElems.insert(*i);
		    }
		    }
		}
		
	    } 
	} //if size-valuecheck>0
    }
    computeFinal = true;
}


void
CheckStore::dump() {
	dumpCheckCode();
	if (computeFinal) {
		errs() <<"-StoreFinal(";
		for (std::set<Value*>::iterator i = finalElems.begin(), e=finalElems.end(); i!=e; i++) {
			errs() << (*i)->getName() <<" ";
		}
		errs() << ")";
	} 
	errs() << "\n";
}


////////////////////////////
//Class CheckBranch       //
////////////////////////////
CheckBranch::CheckBranch(Instruction *I)
    :CheckCode(I) {
    propCheckList.clear();
    propToList.clear();
    isOrigList.clear();
}


CheckBranch::~CheckBranch() {
    CheckElem.clear();
    propCheckList.clear();
    propToList.clear();
    isOrigList.clear();
}

unsigned int
CheckBranch::getOrigNumElem() {
    return origNumElem;
}

std::set<Value*>&
CheckBranch::getCheckElemList() {
    return CheckElem;
}

unsigned int
CheckBranch::getFinalNumElem() {
    return CheckElem.size();
}

void
CheckBranch::dump() {
    dumpCheckCode();
    dump_propCheck();
    std::cerr <<"\n";
}


      ////////////////////////////////
      // class CheckCodeMap         //
      ////////////////////////////////
CheckCodeMap::~CheckCodeMap() {
    //clear checkCodeMap, delete all CheckCode objects
    for (std::map<Instruction*, CheckCode*>::iterator i = checkCodeMap.begin(), e=checkCodeMap.end(); i!=e; i++) {
	CheckCode * checkcodeobj = (*i).second;
	assert(checkcodeobj && "CheckCode obj can not be null");
	delete checkcodeobj;
    }
    checkCodeMap.clear();
}

//return null, if does not find
CheckCode *
CheckCodeMap::getCheckCode(Instruction *I) {
    if (checkCodeMap.find(I) != checkCodeMap.end()) {
	CheckCode *c = checkCodeMap[I];
	assert(c && "checkCode in map, but is null");
	return c;
    } else 
	return NULL;
}

CheckCode *
CheckCodeMap::newCheckCode(Instruction *I) {
    assert((checkCodeMap.find(I) == checkCodeMap.end()) && "There already is an entry in checkCodeMap");
    CheckCode *checkcode;
    if (isa<LoadInst>(I)) 
	checkcode = new CheckLoad(I);
    else if (isa<BranchInst>(I))
	checkcode = new CheckBranch(I);
    else if (isa<StoreInst>(I))  {
#ifdef L3_CHECK
	checkcode = new CheckStore(I);  //- L3
#endif
#ifdef L1_CHECK
	checkcode = new CheckCode(I);   //L1
#endif
    }
    else if (isa<CallInst>(I) || isa <ReturnInst>(I))
	checkcode = new CheckCode(I);
    else 
	assert(0 && "I's type is not recognized. Can not create CheckCode for this I");

    //insert a new entry
    checkCodeMap[I] = checkcode;
    return checkcode;
}

void 
CheckCodeMap::deleteElem(Instruction* I, Value *elem) {
    CheckCode *checkcodeI = getCheckCode(I);
    assert(checkcodeI && "I does not have an entryin CheckCodeMap!");

#ifdef R_DEBUG
    errs() <<"deleteElem("<<elem->getName()<<")From(";
    if (I->hasName()) errs() << I->getName();
    else errs() << I->getParent()->getName() <<":"<<I->getOpcodeName();
    errs()<<")  ";
#endif

    checkcodeI->deleteElement(elem);    
}

//return null, if I does not have an entry in map
std::set<Value*> *
CheckCodeMap::getCheckElemList(Instruction *I) {
	CheckCode * checkList = getCheckCode(I);
	if (checkList) {
	  return &(checkList->getCheckElemList());
	} else 
	  return NULL;
}

void
CheckCodeMap::dump() {
    std::cerr << "======= dump CheckCodeMap ======\n";
    std::map<Instruction*, CheckCode*>::iterator i,e;
    for (i= checkCodeMap.begin(), e=checkCodeMap.end(); i!=e; i++) {
	((*i).second)->dump();
    }
    std::cerr << "\n";  
}



      /////////////////////////////////
      //Class ValueCheckedAt         //
      /////////////////////////////////
bool
ValueCheckedAt::propagateTo(ValueCheckedAt *laterCheck) {
    assert(laterCheck && "laterCheck can not be null");
    bool changed = false;

    std::set<Instruction*>::iterator i,e;

#ifdef R_DEBUG
    //std::cerr << CheckElem->getName() << ": propagate from "<<laterCheck->CheckElem->getName() <<". Before prop ----\n" ;
//    dump();
  //  laterCheck->dump();    
#endif

    //copy laterCheck->CheckedAtList to my PropOrFinalList
    for (i = laterCheck->CheckedAtList.begin(), e = laterCheck->CheckedAtList.end(); i!=e; i++) {
	if (CheckedAtList.find(*i) == CheckedAtList.end()) {
	    if ((PropOrFinalList.insert(*i)).second) changed = true;
	    
	}
    }
    
    //copy laterCheck->PropOrFinalList to my PropOrFinalList
    for (i = laterCheck->PropOrFinalList.begin(), e = laterCheck->PropOrFinalList.end(); i!=e; i++) {
	if (CheckedAtList.find(*i) == CheckedAtList.end()) {
	   if ((PropOrFinalList.insert(*i)).second) changed = true;
	}
    }

#ifdef R_DEBUG
   // std::cerr <<"After prop ---- \n";
    //dump();
#endif

    return changed;

}


void
ValueCheckedAt::dump() {
	errs() << "I("<<CheckElem->getName() <<")-CheckedAt[";
	std::set<Instruction*>::iterator i,e;
	for (i=CheckedAtList.begin(), e=CheckedAtList.end(); i!=e; i++) {
		if ((*i)->hasName())
			errs() << (*i)->getName()<<",";
		else 
			errs() << (*i)->getParent()->getName() <<":"<<(*i)->getOpcodeName()<<",";
	}

	errs() <<"]-Prop[";
	for (i=PropOrFinalList.begin(), e=PropOrFinalList.end(); i!=e; i++) {
		if ((*i)->hasName())
			errs() << (*i)->getName()<<",";
		else 
			errs() << (*i)->getParent()->getName() <<":"<<(*i)->getOpcodeName()<<",";
	}
	errs() <<"]\n";
}


      ////////////////////////////////
      //Class ValueCheckedAtMap     //
      ////////////////////////////////
ValueCheckedAtMap::~ValueCheckedAtMap() {
    //clear valueCheckedAtMap, delete all ValueCheckedAt objects
    std::map<Value *, ValueCheckedAt*>::iterator i,e;
    for (i=valueCheckedAtMap.begin(), e=valueCheckedAtMap.end(); i!=e; i++) {
	ValueCheckedAt * obj = (*i).second;
	assert(obj && "ValueCheckedAt obj can not be null");
	delete (obj);
    }
    valueCheckedAtMap.clear();    
}


void
ValueCheckedAtMap::insertCheckedAt(Value *elem, Instruction *I) {
    //if elem does not have a ValueCheckedAt entry on map, create a new entry
    ValueCheckedAt * checkedat = getValueCheckedTable(elem);
    checkedat->insertCheckedAt(I);
}

void 
ValueCheckedAtMap::insertPropOrFinal(Value *elem, Instruction*I) {
    //if elem does not have a ValueCheckedAt entry on map, create a new entry
    ValueCheckedAt * checkedat = getValueCheckedTable(elem); 
    checkedat->insertPropOrFinal(I);
}

bool
ValueCheckedAtMap::contain(Value* elem) {
    if (valueCheckedAtMap.find(elem) == valueCheckedAtMap.end())
	return false;
    else
	return true;
}


void 
ValueCheckedAtMap::dump() {
    std::cerr << "====dump ValueCheckedAtMap===\n";
    std::map<Value *, ValueCheckedAt*>::iterator i,e;
    for (i=valueCheckedAtMap.begin(), e=valueCheckedAtMap.end(); i!=e; i++) {
	((*i).second)->dump();
    }
    std::cerr <<"\n";
}

ValueCheckedAt*
ValueCheckedAtMap::getValueCheckedTable(Value *elem) {
    if (contain(elem))
	return valueCheckedAtMap[elem];
    else {
	ValueCheckedAt * checkedat = new ValueCheckedAt(elem);
	valueCheckedAtMap[elem] = checkedat;
	return checkedat;
    }
}

void
ValueCheckedAtMap::shrink(std::set<Value*> &keystokeep) {
    //Cut off table. Just keep entreis with keys in keystokeep set
    assert(!keystokeep.empty() && "keystokeep can not be empty");

    std::set<Value*> keysToErase;
    keysToErase.clear();

    std::map<Value*,ValueCheckedAt*>::iterator i = valueCheckedAtMap.begin(), e = valueCheckedAtMap.end();
    for (;i!=e; i++) {
	if (keystokeep.find((*i).first) == keystokeep.end()) {
	    keysToErase.insert((*i).first);
	}
    }
    
    std::set<Value*>::iterator ii = keysToErase.begin(), ie = keysToErase.end();
    for (; ii!=ie; ii++) {
	valueCheckedAtMap.erase(*ii);
    }
}

void
ValueCheckedAtMap::getAllKeys(std::set<Value*> &allkeys) {
  	allkeys.clear();
	std::map<Value*, ValueCheckedAt*>::iterator i = valueCheckedAtMap.begin(), e= valueCheckedAtMap.end();
	for (;i!=e; i++) 
		allkeys.insert((*i).first);

}









//////////////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////
      //  RedundAnalysis class           //
      /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

//Constructor
RedundAnalysis::RedundAnalysis() {
    localnumtotalldcheck=0;
    localnumtotalstcheck=0;
    localnumtotalbrcheck=0;
    localnumtotalothercheck=0;

    reg_safe = false;

    BBtotalN = 0;
    BBIDmap.clear();
    DirtyBBID.clear();
    DirtyBBset.clear();
    connectTable.clear();
    dirtyTable.clear();

}


RedundAnalysis::~RedundAnalysis() {
    assert(ToUpdateList.empty() && "ToUpdateList must be empty");
    //should not clear checkcodemap or valuecheckedatmap
}

///////////////////////////////////////////////////////
///    SetUpTable                                    //
///////////////////////////////////////////////////////

void
RedundAnalysis::SetUpTable(CheckCodeMap *checkCodeMap, ValueCheckedAtMap *valueCheckedAtMap, Function &F) {
	MycheckCodeMap =  checkCodeMap;
	MyvalueCheckedAtMap =  valueCheckedAtMap;
	MyF = &F;

	setupFuncArguSet(F);
	initSynchPoint(F);
	//make sure both maps are empty
	assert(checkCodeMap->empty() && "checkCodeMap must be empty before calling RedundAnalysis");
	assert(valueCheckedAtMap->empty() && "valueCheckedAtMap must be empty before calling RedundAnalysis");

	ToUpdateList.clear();

	ScanAllCheckCodes(F);
#ifdef R_DEBUG
	errs() << "\n\n==== Function "<< F.getName() <<" ====\n";
	errs() << "After ScanAllCheckCodes() inside RedundAnalysis--\n";
	checkCodeMap->dump();
	valueCheckedAtMap->dump();
#endif

	buildSynchPointTable();

	if (!ToUpdateList.empty()) {
		PropagateChecks();
#ifdef R_DEBUG
		errs() << "====== After PropagateChecks =====\n";
		valueCheckedAtMap->dump();
#endif
	}
}


void 
RedundAnalysis::addtoUpdateList(Value *v) {
    //add v to updatelist only if v is instruction and it can prop error
    if (Instruction *I = dyn_cast<Instruction>(v)) {
	if (canPropErrorInst(I)) {
	    //insert I to ToUpdateList if I is not there
	    std::list<Instruction*>::iterator i,e;
	    i = ToUpdateList.begin();
	    e = ToUpdateList.end();
	    while (i!=e && (*i)!=I) i++;
	    if (i==e) 
		ToUpdateList.push_back(I);
	}
    }
}


bool
RedundAnalysis::canPropErrorInst(Instruction *I) {
    if (isCheckPoint(I) || isa<AllocaInst>(I) || isa<VAArgInst>(I) ) return false;
	 if (CallInst* CI = dyn_cast<CallInst>(I))
		 if( CI->getCalledValue()->getName() == "free") 
			 return false;
    //SETXX
	 //FIXME : xiehuc unknow set cond instr
    //if (isa<SetCondInst>(I)) return false;
    return true;
}

bool 
RedundAnalysis::isCheckPoint(Instruction *Ins) {
    if (isa<CallInst>(Ins) || isa<TerminatorInst>(Ins) || isa<StoreInst>(Ins) ||isa<LoadInst>(Ins) ) return true;
    return false;
}


//This function scans all BBs. But it will reorder some intructions inside BB,
//to make condition next to branch.
void 
RedundAnalysis::ScanAllCheckCodes(Function &F) 
{

	for (Function::iterator BBi = F.begin(), BBE = F.end(); BBi!=BBE; ++BBi) {
		BasicBlock *BB = BBi;
		BasicBlock::iterator nextI, I = BB->begin();
		Instruction *LastCond = findLastCond(BB);

		bool isSynchpoint = false;
		while (&*I!=LastCond) {
			nextI = I;
			++nextI;
			if (isCheckPoint(I)) { SetupTablewithCheckPoint(I, BB);}
			if (isSynchPoint(I)) isSynchpoint = true;
			I = nextI;
		}

		//if I is null, means this block has ended
		//do with branch
		//FIXME : xiehuc I couldn't be null
		//if (I!=NULL) {
			if (BranchInst *BI = hasConditionalBr(BB)) {
#ifdef CHECK_BRANCH_OPERANDS
				//Check branch operands
				SetupTablewithBranch(LastCond, BI, BB);
#endif
			} else if (ReturnInst *RI = dyn_cast<ReturnInst>(LastCond)) {
				SetupTablewithReturn(RI, BB);
			}
		//}

		//set up synchpoint analysis
		if (isSynchpoint) markBBdirty(BB);
		addPathFromBB(BB);

	}  //end of for
}


/////////////////////////////////////////////////////////////////////
//////    Set up Table with check points                           //
/////////////////////////////////////////////////////////////////////
void 
RedundAnalysis::SetupTablewithCheckPoint(Instruction*I, BasicBlock*BB) {
#ifdef R_DEBUG
	errs() << "SetupCP(";
	if (I->hasName()) errs()<< I->getName() <<") ";
	else errs() << I->getOpcodeName() <<") ";
	errs() << ":BB{"<<I->getParent()->getName()<<"} ";

#endif

	if (LoadInst *LoadI = dyn_cast<LoadInst>(I)) {
		SetupTablewithLoad(LoadI, BB);
	} else if (StoreInst *StoreI = dyn_cast<StoreInst>(I)) {
#ifdef L3_CHECK
		SetupTablewithStore(StoreI, BB,  CHECKEDAT); //L3 CHECKEDAT
#endif
#ifdef L1_CHECK
		SetupTablewithStore(StoreI, BB, PROPORFINAL); //L1 PROPORFINAL
#endif
	} else if (CallInst *CallI = dyn_cast<CallInst>(I)) {
		SetupTablewithCall(CallI, BB, PROPORFINAL);
	} else {
		assert (0 && "I is not recognized here");
	}
}

void 
RedundAnalysis::SetupTablewithBranch(Instruction *LastCond, BranchInst* BI, BasicBlock*BB){
	assert(BI->isConditional() && "Only conditional branch will be replicated");
	assert((LastCond->getParent())==BB && "LastCond's parent must be BB");

	Instruction *myCond = dyn_cast<Instruction>(BI->getCondition());
	assert (myCond && "Branch condition must not be trivial"); 

	bool checkOperand = false; //indicate whether check two operands or just a bool value

	if (LastCond != BI) {
		//condition must be inside BB, and next to BI
		//If myCond is SETXX, we check its operands
		//FIXME : xiehuc unknow set cond inst
#if 0
		if (isa<SetCondInst>(myCond)) {
			checkOperand = true;
			Value *op0 = myCond->getOperand(0);
			Value *op1 = myCond->getOperand(1);

			if (duplicable(op0) || duplicable(op1)) {
#ifdef R_DEBUG
				std::cerr << "SetupBr-setxx:BB{"<< BI->getParent()->getName()<<"} ";
#endif
				CheckCode *checkcodeEntry = MycheckCodeMap->newCheckCode(BI);
				assert(checkcodeEntry && "Must not be null");

				if (duplicable(op0)) {
					SetUpTablewithOP(checkcodeEntry, op0, BI, CHECKEDAT);
					localnumtotalbrcheck++;
				}

				if (duplicable(op1)) {
					SetUpTablewithOP(checkcodeEntry, op1, BI, CHECKEDAT);
					localnumtotalbrcheck++;
				}
			}
		}
#endif
	}

	//if the condition's operand is not checked, we need to check the bool value
	if (!checkOperand) {
		if (duplicable(myCond)) {
#ifdef R_DEBUG
			errs() << "SetupBr-bool:BB{"<< BI->getParent()->getName()<<"} ";
#endif
			CheckCode *checkcodeEntry = MycheckCodeMap->newCheckCode(BI);
			assert(checkcodeEntry && "Must not be null");
			SetUpTablewithOP(checkcodeEntry, myCond, BI, CHECKEDAT);
			localnumtotalbrcheck++;
		}
	}
}

void 
RedundAnalysis::SetupTablewithReturn(ReturnInst *returnI, BasicBlock* BB){
	Value *retV = returnI->getReturnValue();
	LLVMContext& C = BB->getContext();
	if (retV && (retV->getType() != Type::getVoidTy(C))) {
		if (duplicable(retV)) {
			CheckCode *checkcodeEntry = MycheckCodeMap->newCheckCode(returnI);
			assert(checkcodeEntry && "Must not be null");
			SetUpTablewithOP(checkcodeEntry, retV, returnI, PROPORFINAL);
			localnumtotalothercheck++;
		}
	}
}

void
RedundAnalysis::SetupTablewithLoad (LoadInst *loadI, BasicBlock *BB) {
    Value *addrP = loadI->getOperand(0);
    if (duplicable(addrP)) {
	CheckCode *checkcodeEntry = MycheckCodeMap->newCheckCode(loadI);
	SetUpTablewithOP(checkcodeEntry, addrP, loadI, CHECKEDAT);

	//decompose addrP
	std::set<Value*> deValues;
#ifdef DECOMPOSE_ADDR
	DecomposeAddress(deValues,addrP);
#endif
	//stat
	if (deValues.empty()) localnumtotalldcheck++;
	else localnumtotalldcheck += deValues.size();

	//insert decomposedValues to tables
	if (!deValues.empty()) {
	    for (std::set<Value*>::iterator i=deValues.begin(), e=deValues.end(); i!=e; i++) {
		assert(duplicable(*i) && "decomposed value must be duplicable");
		SetUpTablewithOP(checkcodeEntry, *i, loadI, CHECKEDAT);
	    }
	}
    }
}

void 
RedundAnalysis::SetupTablewithStore(StoreInst *storeI, BasicBlock *BB, enum CHECKTYPE checktype) {
    Value *addrP = storeI->getPointerOperand();
    Value *StoreValue = storeI->getOperand(0);

    if (duplicable(addrP) || duplicable(StoreValue)) {
	CheckCode *checkcodeEntry = MycheckCodeMap->newCheckCode(storeI);
	assert(checkcodeEntry && "Must not be null");
	
	if (duplicable(StoreValue)) {
	    SetUpTablewithOP(checkcodeEntry, StoreValue, storeI, checktype);
	    localnumtotalstcheck ++;
	}

	
	if (duplicable(addrP)) {
	    SetUpTablewithOP(checkcodeEntry, addrP, storeI, checktype);
	    
	    //decompose addrP
	    std::set<Value*> deValues;
	    deValues.clear();
	  

	    //Jing - ugly ----!!!!! Do not decompose address 
	    //if store is synchpoint
#ifdef L1_CHECK
	    localnumtotalstcheck++; //L1
#endif

#ifdef L3_CHECK
	    // L3
#ifdef DECOMPOSE_ADDR
	    DecomposeAddress(deValues,addrP);  //L3
#endif
	    if (deValues.empty()) localnumtotalstcheck++; //L3
	    else localnumtotalstcheck+=deValues.size(); //L3
#endif

	    //insert decomposedValues to tables
	    if (!deValues.empty()) {
		for (std::set<Value*>::iterator i=deValues.begin(), e=deValues.end(); i!=e; i++) {
		    assert(duplicable(*i) && "decomposed value must be duplicable");
		    SetUpTablewithOP(checkcodeEntry, *i, storeI, checktype);
		}
	    }
	}//addrP
    }
}

void 
RedundAnalysis::SetupTablewithCall(CallInst *callI, BasicBlock *BB, enum CHECKTYPE checktype) {
	//parameter starts from 1 to NumOperands-1
	int numParam = callI->getNumOperands() - 1;
	CheckCode *checkcodeEntry = MycheckCodeMap->newCheckCode(callI);
	assert(checkcodeEntry && "Must not be null");

	for (int i = 1; i <= numParam; i++ ) {
	    Value *param = callI->getOperand(i);
	    
	    if ( duplicable(param)) {
		SetUpTablewithOP(checkcodeEntry, param, callI, checktype);
		localnumtotalothercheck++;
	    }
	}
}

void
RedundAnalysis::SetUpTablewithOP(CheckCode *checkcodeEntry, Value *v, Instruction *I, enum CHECKTYPE type) {
    checkcodeEntry->insertOrigElement(v);
    
    if (type == PROPORFINAL) 
	MyvalueCheckedAtMap->insertPropOrFinal(v, I);
    else if (type == CHECKEDAT)
	MyvalueCheckedAtMap->insertCheckedAt(v, I);
    
    addtoUpdateList(v);
}

void RedundAnalysis::printStatforTotal(Function &F){
	errs() << "LOCAL_REDUND_CHECK "<< localnumtotalldcheck <<" localnumtotalldcheck ("<<F.getName()<<")\n";
	errs() << "LOCAL_REDUND_CHECK "<< localnumtotalstcheck <<" localnumtotalstcheck ("<<F.getName()<<")\n";
	errs() << "LOCAL_REDUND_CHECK "<< localnumtotalbrcheck <<" localnumtotalbrcheck ("<<F.getName()<<")\n";
	errs() << "LOCAL_REDUND_CHECK "<< localnumtotalothercheck <<" localnumtotalothercheck ("<<F.getName()<<")\n";

	//clear counters
	localnumtotalldcheck=0;
	localnumtotalstcheck=0;
	localnumtotalbrcheck=0;
	localnumtotalothercheck=0;
}

///////////////////////////////////////////////////////////
///// Propagate Checks                               ////
/////////////////////////////////////////////////////////
void
RedundAnalysis::PropagateChecks() {
    //keep a list of vars directly checked by checking point
    std::set<Value*> vartocheck;
    MyvalueCheckedAtMap->getAllKeys(vartocheck);

    //propagation. During this time, MyvalueCheckedAtMap could become very large
    while (!(ToUpdateList.empty())) {
	Instruction *ins = ToUpdateList.front();
	ToUpdateList.pop_front();

	if (MyvalueCheckedAtMap->contain(ins)) {
	    ValueCheckedAt *checkedSet_ins = MyvalueCheckedAtMap->getValueCheckedTable(ins);
	
	    for (unsigned int i = 0; i< ins->getNumOperands(); i++) {
		Value *op = ins->getOperand(i);
		if (duplicable(op)) {
		    ValueCheckedAt *checkedSet_op = MyvalueCheckedAtMap->getValueCheckedTable(op);
		    bool changed = checkedSet_op->propagateTo(checkedSet_ins);
		    if (changed) addtoUpdateList(op);
		}
	    }//end of for
	}
    }//end of while

    //shrink MyvalueCheckedAtMap. Cut off entries not in vartocheck set
    if (!vartocheck.empty()) MyvalueCheckedAtMap->shrink(vartocheck);

}





///////////////////////////////////////////////////////
///    SynchPoint Analysis                           //
///////////////////////////////////////////////////////
bool 
RedundAnalysis::isSynchPoint(Instruction *I) {
#ifdef L1_CHECK
    //  L1
	if (isa<StoreInst>(I) || isa<CallInst>(I))
		return true;
	else
		return false;
#endif
#ifdef L3_CHECK
    return (isa<CallInst>(I));  //L3
#endif
}


void
RedundAnalysis::initSynchPoint(Function &F) {
    int id = 0;
    for (Function::iterator BBi = F.begin(), BBE = F.end(); BBi!=BBE; ++BBi) {
	BasicBlock *BB = BBi;
	BBIDmap[BB] = id;
	id++;
    }
    BBtotalN = id;
#ifdef R_DEBUG
    std::cerr << "initSynchPoint -- BBtotalN = "<<BBtotalN <<"\n";
#endif
    for (int i = 0; i < BBtotalN * BBtotalN; i++) {
	connectTable.push_back(0);
	dirtyTable.push_back(0);
	//connectTable[i] = 0;
	//dirtyTable[i] = 0;
    }
}


int
RedundAnalysis::getBBID(BasicBlock*BB) {
    //comment it for fast execution
    //assert(BBIDmap.find(BB) != BBIDmap.end() && "BB must be in BBIDmap");
  return BBIDmap[BB];
}

void
RedundAnalysis::addPathFromBB(BasicBlock *BB) {
	TerminatorInst *TI = BB->getTerminator();
	if (BranchInst *BI = dyn_cast<BranchInst>(TI)) {
		unsigned int num = BI->getNumSuccessors();
		for (unsigned int i = 0; i < num; i++) 
		   addPathtoCTable(BB, BI->getSuccessor(i) );
	}
}

void
RedundAnalysis::addPathtoCTable(BasicBlock *b1, BasicBlock *b2) {
	int i1 = getBBID(b1);
	int i2 = getBBID(b2);
	connectTable[i1*BBtotalN+i2] = 1;
}

void 
RedundAnalysis::markBBdirty(BasicBlock*B) {
	if (DirtyBBset.find(B) == DirtyBBset.end()) {
		DirtyBBset.insert(B);
		DirtyBBID.push_back(getBBID(B));
	}
}

void
RedundAnalysis::buildSynchPointTable(){
	//build connectivity table
	buildCTable();
#ifdef R_DEBUG
	std::cerr << "Successfully build connectTable\n";
#endif
	//build synchpoint data base
	buildDTable();
#ifdef R_DEBUG
	std::cerr << "Successfully build dirtyTable\n";
#endif
}

void
RedundAnalysis::buildCTable(){
  int k, i, j;
  for (k = 0; k < BBtotalN; k++) {
	for (i = 0; i < BBtotalN; i++) {
	    for (j = 0; j < BBtotalN; j++) {
		connectTable[i*BBtotalN+j] = connectTable[i*BBtotalN+j] |
		    (connectTable[i*BBtotalN+k] & connectTable[k*BBtotalN+j]);
	
	    }
	}
  }
}

void
RedundAnalysis::buildDTable() {
 int k, i, j, t;
 int sizedirtyB = DirtyBBID.size();
 for (i = 0; i < BBtotalN; i++) {
   for (j = 0; j < BBtotalN; j++) {
 	   for (t = 0; t < sizedirtyB; t++) {
	   		k = DirtyBBID[t];
			dirtyTable[i*BBtotalN+j] = dirtyTable[i*BBtotalN+j] |
				(connectTable[i*BBtotalN+k] & connectTable[k*BBtotalN+j]);
	
		}
	}
 }

}

bool
RedundAnalysis::hasSynchPoint(BasicBlock* sB, BasicBlock *eB) {
	int s1 = getBBID(sB);
	int s2 = getBBID(eB);
	if (dirtyTable[s1*BBtotalN+s2]) return true;
	else return false;
}

bool
RedundAnalysis::hasSynchPoint(Instruction *sI, Instruction *eI) {
	BasicBlock *sB = sI->getParent(), *eB = eI->getParent();
	if (sB == eB) 
	  return hasSynchPointWithinBB(sI,eI);

	// Now sB != eB
	if (hasSynchPoint(sB, eB))
		return true;

	//if there is syn from SI to BB's end
	if (hasSynchPointWithinBB(sI, &(sB->back())))
	    return true;
	//if there is syn from BB's begin to eI
	if (isSynchPoint(eB->begin()) ||
	    hasSynchPointWithinBB(eB->begin(), eI))
	    return true;

	return false;
}

//not include s and e
bool 
RedundAnalysis::hasSynchPointWithinBB(Instruction* s, Instruction* e) {

	BasicBlock *BB = s->getParent();
	assert(BB == e->getParent() && "Two ins must within the same BB");

	Instruction* lastInst = &BB->back();
	if (s == BB->getTerminator()) return false;

	Instruction *I = s->getNextNode();

	while (I != e && I != lastInst) {
		if (isSynchPoint(I)) return true;
		I = I->getNextNode();
	}
	//actually, terminator is not synchpoint
	//if (I != e ) return isSynchPoint(lastInst);
	return false;
}



bool
RedundAnalysis::hasSynchPoint(Loop *loop) {
    // For loop, we check if all BB inside have synchpoint.
    std::vector<BasicBlock*>::const_iterator blockI = loop->block_begin(),
											 blockE = loop->block_end();
	while (blockI != blockE) {
		BasicBlock *BB = *blockI;
		if (hasSynchPoint(BB)) return true;
		blockI++;
	}
	return false;
}

bool
RedundAnalysis::hasSynchPoint(BasicBlock *bb) {
	if (DirtyBBset.find(bb) == DirtyBBset.end()) return false;
	else return true;
}









///////////////////////////////////////////////////////
///    removeOverlap                                 //
///////////////////////////////////////////////////////
//removeOverlap has to be inside RedundAnalysis class. Otherwise, those 
//ugly helper functions need to be copied again.

void 
RedundAnalysis::removeOverlap(CheckCodeMap *checkCodeMap, 
		ValueCheckedAtMap *valueCheckedAtMap, 
		Function &F, 
		PostDominatorSet &postdominSet) {
    assert(&F == MyF && "Function changed");
    assert(checkCodeMap == MycheckCodeMap && "MycheckCodeMap changed");
    assert(valueCheckedAtMap == MyvalueCheckedAtMap 
	   && "MyvalueCheckedAtMap changed");

    bool changed = false;

    if (!MyvalueCheckedAtMap->empty()) {
#ifdef R_DEBUG
	errs() << "\n============Remove overlapped checked ("<< F.getName();
	errs() <<") ==============\n";
#endif
	std::set<Value*> allkeys;
	MyvalueCheckedAtMap->getAllKeys(allkeys);
	
	for (std::set<Value*>::iterator keyi = allkeys.begin(), 
		 keye = allkeys.end(); keyi != keye; keyi++) {
	    ValueCheckedAt *checkedAt = MyvalueCheckedAtMap->getValueCheckedTable(*keyi);
#ifdef R_DEBUG
	    //errs() << "removeAtValue(" << (*keyi)->getName()<<")----";
#endif
	    if (removeOverlapOnValue((*keyi),checkedAt, postdominSet))
		changed = true;
#ifdef R_DEBUG
	    //errs() <<"\n";
#endif
	}
    
#ifdef R_DEBUG
	if (changed) {
	    MycheckCodeMap->dump();
	    MyvalueCheckedAtMap->dump();
	} else 
	    std::cerr << "No Change!!!!!\n";
#endif
    } //end of if !empty
    
}


bool
RedundAnalysis::removeOverlapOnValue(Value*v,ValueCheckedAt* checkatTable,PostDominatorSet &postdominSet) {
    bool changed = false;

    if (!(checkatTable->notRemovable()) ) {
	std::set<Instruction*> toErase;
	toErase.clear();
	
	std::set<Instruction*> &CheckedAtList = checkatTable->CheckedAtList;
	std::set<Instruction*> &PropOrFinalList = checkatTable->PropOrFinalList;
	std::set<Instruction*>::iterator i,ie,j,je;

	for (i = CheckedAtList.begin(), ie = CheckedAtList.end(); i != ie; i++) {
	    Instruction *hereMycheck = (*i);

	    //debug - jing
#ifdef R_DEBUG
	    //std::cerr <<"myCheckisOnBB("<< (*i)->getParent()->getName()<<") ";
#endif	    

	    //some weired errors: on craft:main(), one block is claimed to not in the function!
	    if (postdominSet.find(hereMycheck->getParent())==postdominSet.end()) continue;
	    //there are still some checks not reachable, fine...
	    if (!(postdominSet.isReachable(hereMycheck->getParent()))) 
		//not reachable. then we don't touch it. -- to be safe	
		continue;
	    
	    //assert(postdominSet.isReachable(hereMycheck->getParent()) && 
	    //	   "hereMycheck must be reachable");
	    bool erased = false;

	    //check PropOrFinalList
	    for (j = PropOrFinalList.begin(), je = PropOrFinalList.end(); j!=je; j++) {
		Instruction *latercheck = (*j);
		assert(hereMycheck != latercheck && "PropOrFinalList should not overlap with CheckedAtList");
		if (postdominSet.dominates(latercheck, hereMycheck)) 
		    if (!hasSynchPoint(hereMycheck,latercheck))  {
			erased = true;
			break;
		}
	    } //end of for j
	    
	    //check CheckedAtList
	    if (!erased) {
		for (j = CheckedAtList.begin(), je = CheckedAtList.end(); j!=je; j++) {
		    Instruction *latercheck = (*j);
		    if (hereMycheck != latercheck) {
			if (postdominSet.dominates(latercheck, hereMycheck)) 
			  if (!hasSynchPoint(hereMycheck,latercheck))  {
//REG_SAFE
			    if (reg_safe) {
				  // if hereMycheck does not dominate latercheck, we anyway
				  // can not apply advanced reg safe optimization.
				  if (!(DominSet->dominates(hereMycheck, latercheck))) {
					erased = true;
					break;
				  }
//end REG_SAFE
				} else {
				  erased = true;
				  break;
				}
			  }
		    }

		} //end of for j
		
	    } //end of erased

	    if (erased) {
		toErase.insert(hereMycheck);
	    }

	} //end of for i

	//Actual erase elements from two tables
	if (!toErase.empty()) {
	    for (i = toErase.begin(), ie = toErase.end(); i!=ie; i++) {
		//delete v from (*i)'s entry
		MycheckCodeMap->deleteElem((*i), v);
		//delete (*i) from v's entry
		CheckedAtList.erase(*i);
	    }
	    changed = true;
	}

    } // end of notRemovable
    return changed;
}




///////////////////////////////////////////////////////
///    DecomposeAddress                          //
///////////////////////////////////////////////////////
//decompose one level
void
RedundAnalysis::DecomposeAddress(std::set<Value*>&valueSet, Value *addrP) {
    valueSet.clear();
    assert(duplicable(addrP) && "addr of load must be duplicable here");
    Instruction *addrI = dyn_cast<Instruction>(addrP);
    //only instruction is able to be decomposed
    if (addrI) {
	if (Value *castOperand = scanCast(addrI)) {
	if (Instruction *addr = dyn_cast<Instruction>(castOperand)) {
	    if (GetElementPtrInst *pointer = 
		dyn_cast<GetElementPtrInst>(addr)) {
		//GetElementPtr
		for (unsigned int i = 0; i< pointer->getNumOperands(); i++) {
		    Value *op = pointer->getOperand(i);
		    Value *castop = scanCast(op);
		    if (castop) valueSet.insert(castop);
		}
	    } else
		if (addr->getOpcode() == Instruction::Add ||
		    addr->getOpcode() == Instruction::Sub) {
		    for (unsigned int i = 0; i<addr->getNumOperands(); i++) {
			Value *op = addr->getOperand(i);
			Value *castop = scanCast(op);
			if (castop) valueSet.insert(castop);
		    }

		} else {
		    //normal inst
		    if (castOperand != addrI) //has been casted
			valueSet.insert(castOperand);
		}
	} else {//end of inst-cast
	    //duplicable value. In this case, castOperand != addrI
	    valueSet.insert(castOperand);
	} //not duplicable addrI or casted value

	} //end of scancast
    } //end of addrI
}

Value*
RedundAnalysis::scanCast(Value *I) {
    //remove scan wrap. return null if not duplicable
    Value *returnV = I;
    if (CastInst *castI = dyn_cast<CastInst>(I)) {
	returnV = castI->getOperand(0);
    }
    if (duplicable(returnV)) return returnV;
    else return NULL;
}







///////////////////////////////////////////////////////
///    regsafe                                       //
///////////////////////////////////////////////////////
//This pass will make valueCheckedAtMap and checkCodeMap inconsistent
//It has be called in the last
void
RedundAnalysis::rmSafeReg(CheckCodeMap *checkCodeMap, ValueCheckedAtMap *valueCheckedAtMap, Function &F) {
    assert(&F == MyF && "Function changed");
    assert(checkCodeMap == MycheckCodeMap && "MycheckCodeMap changed");
    assert(valueCheckedAtMap == MyvalueCheckedAtMap 
	   && "MyvalueCheckedAtMap changed");

    localnumsaferegld=0;
    localnumsaferegst=0;
    localnumsaferegbr=0;
    localnumsaferegother=0;

    bool changed = false;

    if (!MyvalueCheckedAtMap->empty()) {
#ifdef R_DEBUG
	errs() << "\n============Remove values in safe register("<< F.getName();
	errs() <<") ==============\n";
#endif

	std::map<Instruction*,CheckCode*>&mycheckCodeMap = checkCodeMap->getMap();
	std::map<Instruction*,CheckCode*>::iterator i,e;
	for (i = mycheckCodeMap.begin(), e=mycheckCodeMap.end(); i!=e;i++) {
	    Instruction *checkforI = (*i).first;
	    CheckCode *checkcode = (*i).second;
	    std::set<Value*>&elemlist = checkcode->getCheckElemList();
	    if (!elemlist.empty()) {
		std::set<Value*> toRemove;
		toRemove.clear();

		for (std::set<Value*>::iterator si = elemlist.begin(), se=elemlist.end(); si!=se;si++) {
		    if (isSafeReg(*si)) {
			toRemove.insert(*si);
		    }
		} //end of search
		//remove these checkings
		if (!toRemove.empty()) {
		    statRegRemove(checkforI, toRemove.size());
		    for (std::set<Value*>::iterator si = toRemove.begin(), se=toRemove.end(); si!=se; si++) {
			elemlist.erase(*si);
		    }
		    changed = true;
		}
		
	    }//end fo not empty elemlist
	}//end of for i
    
	if (changed) {
	    printSafeRegPassStat(F);  
	}    

#ifdef R_DEBUG
	if (changed) {
	    MycheckCodeMap->dump();
	} else 
	    std::cerr << "No Change!!!!!\n";
#endif
    } //end of if !empty   


}

void
RedundAnalysis::statRegRemove(Instruction *I, int num) {
    if (isa<LoadInst>(I)) localnumsaferegld += num;
    else if (isa<StoreInst>(I)) localnumsaferegst += num;
    else if (isa<BranchInst>(I)) localnumsaferegbr += num;
    else localnumsaferegother += num;
}

bool
RedundAnalysis::isSafeReg(Value*v) {
    Value *beforeCast = scanCast(v);
    if (beforeCast && isa<LoadInst>(beforeCast)) 
	return true;
    else return false;
}

void 
RedundAnalysis::printSafeRegPassStat(Function &F) {
     errs() << "LOCAL_REDUND_CHECK "<< localnumsaferegld <<" localnumsaferegld ("<<F.getName()<<")\n";
     errs() << "LOCAL_REDUND_CHECK "<< localnumsaferegst <<" localnumsaferegst ("<<F.getName()<<")\n";
     errs() << "LOCAL_REDUND_CHECK "<< localnumsaferegbr <<" localnumsaferegbr ("<<F.getName()<<")\n";
     errs() << "LOCAL_REDUND_CHECK "<< localnumsaferegother <<" localnumsaferegother ("<<F.getName()<<")\n";   
}


void
RedundAnalysis::checkADVRegSafe(DominatorSet *dominset) {
	reg_safe = true;
	assert(dominset!=NULL && "DominatorSet can not be null");
	DominSet = dominset;
}







///////////////////////////////////////////////////////
///    Loop IV                                       //
///////////////////////////////////////////////////////




      /////////////////////////////////
      //Class LoopIVInfo             //
      /////////////////////////////////
//Implement class LoopIVInfo
LoopIVInfo::LoopIVInfo(Loop *loop) {
    IVset.clear();
    ExitingBranch.clear();
    assert(loop && "Can not initiate LoopIVInfo with NULL");
    myloop = loop;
}

LoopIVInfo::~LoopIVInfo() {
    IVset.clear();
    ExitingBranch.clear();
}


void
LoopIVInfo::addExitingBlocks(std::vector<BasicBlock*>&blocks) {
 	assert(ExitingBranch.empty() && "ExitingBranch set has been polluted");
	assert (blocks.size() <= 2 && "num of exiting blocks must be less than 3");
	if (!blocks.empty()) {
	   for (int i = 0, bsize = blocks.size(); i < bsize; i++) {
		 addExitingBlock(blocks[i]);
	   }
	}
}


void
LoopIVInfo::addExitingBlock(BasicBlock *exitingBB) {
    assert(myloop->contains(exitingBB) && 
	   "exitingBB does not belong to this loop");
    TerminatorInst *TI = exitingBB->getTerminator();
    BranchInst *BranchI = dyn_cast<BranchInst> (TI); 
    //this fail on PAPI 
    if (BranchI && BranchI->isConditional()) {
	//assert(BranchI && BranchI->isConditional() && "Loop exiting must be a conditional branch");
    //check which branch is to outside
	BasicBlock *out0 = BranchI->getSuccessor(0);
	BasicBlock *out1 = BranchI->getSuccessor(1);
    
	if (myloop->contains(out0)) {
	    assert (!(myloop->contains(out1)) && "one edge must go outside loop");
	    addBranch(BranchI, false);
	} else {
	    assert(!(myloop->contains(out0)) && "one edge must go outside loop");
	    addBranch(BranchI, true);
	}

    }
}

void 
LoopIVInfo::addBranch(BranchInst *br, bool toout) {
    assert(ExitingBranch.find(br) == ExitingBranch.end() &&
	   "Exit branch already exists");
    ExitingBranch[br]=toout;
}

void
LoopIVInfo::addIVset(std::vector<PHINode*>&IVs) {
	assert(IVset.empty() && "IVset has been polluted");
	int Isize = IVs.size();
	assert (Isize > 0 && "num of IVs must be positive");
	for (int i = 0; i< Isize; i++) 
		IVset.insert(IVs[i]);
}

void
LoopIVInfo::dump() {
	errs() << "Loop-Head("<<myloop->getHeader()->getName()<<")";
	errs() << "-IVs(";
	for (std::set<Value*>::iterator i = IVset.begin(), e = IVset.end();
		i!=e; i++) 
		errs() <<(*i)->getName()<<",";
	errs() <<")-ExitBranch(";
	for (std::map<BranchInst*,bool>::iterator i = ExitingBranch.begin(),
		e = ExitingBranch.end(); i!=e; i++) 
		errs() << (*i).first->getParent()->getName() <<":"<<(*i).second<<",";
	errs() << ")";
}







      /////////////////////////////////
      //rmLoopIV                     //
      /////////////////////////////////
//remove redundant loop inductin variable
//For this loops, also remove checks on constant.
//After this pass, check(i) are moved from normal tables to checkbranch's propToList.
void 
RedundAnalysis::rmLoopIV(CheckCodeMap *checkCodeMap, ValueCheckedAtMap *valueCheckedAtMap, Function &F, LoopInfo &loopinfo) {
    assert(&F == MyF && "Function changed");
    assert(checkCodeMap == MycheckCodeMap && "MycheckCodeMap changed");
    assert(valueCheckedAtMap == MyvalueCheckedAtMap 
	   && "MyvalueCheckedAtMap changed");

    localnumloopld=0;
    localnumloopst=0;
    localnumloopbr=0;
    localnumloopother=0;


    bool changed = false;
#ifdef R_DEBUG
    errs() << "\n============Remove redundant loopIV ("<< F.getName();
    errs() <<") ==============\n";
#endif

    std::list<Loop*> potentialLoops;
    potentialLoops.clear();
    
    getInnermostLoop(potentialLoops, F, loopinfo);
    if (!potentialLoops.empty()) {
	for (std::list<Loop*>::iterator i=potentialLoops.begin(), e=potentialLoops.end(); i!=e; i++) {
	    if (LoopIVInfo * loopIvInfo = getLoopIV(*i)) {
		if (optimizeOnIV(*i,loopIvInfo)) changed = true;
		
		delete loopIvInfo;
	    }
	}
    } //end of if empty

    if (changed) {
	printLoopOptPassStat(F);
    } else {
#ifdef R_DEBUG
	errs() << "No change...\n";
#endif
	}
}

//if not satisfy, return NULL
LoopIVInfo*
RedundAnalysis::getLoopIV(Loop* loop) {
    //# of Exiting block  <= 2
	SmallVector<BasicBlock*,8> Eblocks;
	Eblocks.clear();
	loop->getExitingBlocks(Eblocks);
	if (Eblocks.size() <= 2) {
		//has IV
		std::vector<PHINode*> IVs;
		IVs.clear();
		getAllInductionVariable(loop, IVs);

		if (IVs.size() > 0) {
		   //no func call or store
		   if (!hasSynchPoint(loop)) {
		       //add loopinfo to LoopIVInfo object
		       LoopIVInfo *info = new LoopIVInfo(loop);
				 std::vector<BasicBlock*> Eblocks__(Eblocks.begin(),Eblocks.end());
		       info->addExitingBlocks(Eblocks__);
		       info->addIVset(IVs);
#ifdef R_DEBUG
		       std::cerr << "LoopSatisfy^^^";
		       info->dump();
		       std::cerr <<"^^^\n";
#endif
		       return info;
		   }
		}
	}
    return NULL;
}

bool
RedundAnalysis::optimizeOnIV(Loop* loop, LoopIVInfo *loopIVinfo) {
    bool changed = false;
    std::set<Value*>::iterator i = loopIVinfo->IVset.begin(), 
	e = loopIVinfo->IVset.end();
    for (; i!=e; i++) {
	Value *iv = (*i);
	if (MyvalueCheckedAtMap->contain(iv)) {
	    ValueCheckedAt *checkedTable = 
		MyvalueCheckedAtMap->getValueCheckedTable(iv);
	    if (!checkedTable->notRemovable()) {
		//pick inst that is inside loop
		std::set<Instruction*>removed;
		removed.clear();

		std::set<Instruction*>::iterator ii = 
		    checkedTable->CheckedAtList.begin(), ie =
		    checkedTable->CheckedAtList.end();
		for (; ii!= ie; ii++) {
		    Instruction *byInst = *ii;
		    if (insideLoop(loop, byInst)) {
			//remove iv from byInst's check-elem list
			MycheckCodeMap->deleteElem(byInst, iv);
#ifdef R_DEBUG
			std::cerr<<"\n";
#endif
			//remove byInst from iv's checkedTable
			removed.insert(byInst);
			statLoopOpt(iv, byInst);

			//check if there is checking on constant
			optConstantCheck(iv, loop,byInst, loopIVinfo);

			changed = true;
		    }//end of inside if
		}//end of for

		if (!removed.empty()) {
		    //copy the check outside of loop
		    moveOutofLoop(iv, loopIVinfo);
		    for (ii = removed.begin(), ie = removed.end(); ii != ie; ii++) 
			checkedTable->CheckedAtList.erase(*ii);
		}
	    } //end of removable
	} //end of inside loop
    } //end of IVset

    return changed;
}

bool
RedundAnalysis::insideLoop(Loop *loop, Instruction *I) {
    return (loop->contains(I->getParent()));
}


//check if checkI checks loop invariants
void 
RedundAnalysis::optConstantCheck(Value *iv, Loop *loop, Instruction *checkI, LoopIVInfo *info) {
    //TODO
    CheckCode *checkcodeforI = MycheckCodeMap->getCheckCode(checkI);
    assert(checkcodeforI && "checkcode must be not null");
    std::set<Value*>&checkElem = checkcodeforI->getCheckElems();
    if (!checkElem.empty()) {
	std::set<Value*> toremove;
	toremove.clear();

	std::set<Value*>::iterator  i=checkElem.begin(), e=checkElem.end();
	for (; i!=e;i++) {
	    if (loop->isLoopInvariant(*i)) {
		//copy checking on loop invariant to outsie
		moveOutofLoop(*i, info);
		//remove checkI from (*i)'s list
		MyvalueCheckedAtMap->getValueCheckedTable(*i)->CheckedAtList.erase(checkI);
		//remove *i from checkI's list
		toremove.insert(checkI);
#ifdef R_DEBUG
		errs() << "Move checking on loop invariant " << (*i)->getName() 
			  << " for I(";
		if (checkI->hasName()) errs() << checkI->getName();
		else errs() << checkI->getParent()->getName() <<":"<<checkI->getOpcodeName();
		errs() <<")\n";
		
#endif
		statLoopOpt(*i, checkI);
	    } //end of if
	}//end of for

	//do actual remove
	if (!toremove.empty()) 
	    for (i=toremove.begin(), e=toremove.end(); i!=e; i++)
		checkElem.erase(*i);
    }
}


///Add iv to exiting branch's partial check list
void
RedundAnalysis::moveOutofLoop(Value *iv, LoopIVInfo *info) {
	//get Exiting blocks
	//may fail with PAPI
	if (!((info->ExitingBranch).empty())) {
		//assert(!((info->ExitingBranch).empty()) && "Exiting blocks should be 0");
		std::map<BranchInst*,bool>::iterator mapi = info->ExitingBranch.begin(),
			mape = info->ExitingBranch.end();
		for (;mapi!=mape; ++mapi) {
			BranchInst *exitBr = (*mapi).first;
			bool outto = (*mapi).second;

			CheckCode * checkcode = MycheckCodeMap->getCheckCode(exitBr);
			if (!checkcode) 
				checkcode = MycheckCodeMap->newCheckCode(exitBr);
			CheckBranch *checkbr = (CheckBranch*)(checkcode);

			checkbr->insertPropCheck(iv, outto, false);


#ifdef R_DEBUG
			errs() << "Copy check on " << iv->getName() << " to Loop outsie BB(" <<
				exitBr->getParent()->getName()<<")-";
			checkbr->dump_propCheck();
			errs() << "\n";
#endif
		}
	}
}



void
RedundAnalysis::statLoopOpt(Value *v, Instruction *I) {
    if (isa<LoadInst>(I)) localnumloopld ++;
    else if (isa<StoreInst>(I)) localnumloopst ++;
    else if (isa<BranchInst>(I)) localnumloopbr ++;
    else localnumloopother ++;
}

void 
RedundAnalysis::printLoopOptPassStat(Function &F) {
	errs() << "LOCAL_REDUND_CHECK "<< localnumloopld <<" localnumloopld ("<<F.getName()<<")\n";
	errs() << "LOCAL_REDUND_CHECK "<< localnumloopst <<" localnumloopst ("<<F.getName()<<")\n";
	errs() << "LOCAL_REDUND_CHECK "<< localnumloopbr <<" localnumloopbr ("<<F.getName()<<")\n";
	errs() << "LOCAL_REDUND_CHECK "<< localnumloopother <<" localnumloopother ("<<F.getName()<<")\n";   
}




///IV: an integer recurrence that increments by a constant
///each time through the loop. If so, return the phi node that corresponds
///to it.
void
RedundAnalysis::getAllInductionVariable(Loop*loop, std::vector<PHINode*>&IVs) {
	IVs.clear();

  	BasicBlock *H = loop->getHeader();

  	BasicBlock *Incoming = 0, *Backedge = 0;
	pred_iterator PI = pred_begin(H);
	assert(PI != pred_end(H) && "Loop must have at least one backedge!");
	Backedge = *PI++;
	if (PI == pred_end(H)) return;  // dead loop
	Incoming = *PI++;
	if (PI != pred_end(H)) return;  // multiple backedges?

	if (loop->contains(Incoming)) {
	    if (loop->contains(Backedge))
	       return ;
	    std::swap(Incoming, Backedge);
	} else if (!(loop->contains(Backedge)))
	      return ;

	// Loop over all of the PHI nodes, looking for a canonical indvar.
	
	for (BasicBlock::iterator I = H->begin(); isa<PHINode>(I); ++I) {
		PHINode *PN = cast<PHINode>(I);
	  if (Instruction *Inc =
	     dyn_cast<Instruction>(PN->getIncomingValueForBlock(Backedge)))
	    if (Inc->getOpcode() == Instruction::Add ||
			Inc->getOpcode() == Instruction::Sub)
		   if (Inc->getOperand(0) == PN)
	         if (isa<ConstantInt>(Inc->getOperand(1)))
		         IVs.push_back(PN);
	}//end of for

}

//get inner most loops
void
RedundAnalysis::getInnermostLoop(std::list<Loop*>&innerLoops, Function&F, LoopInfo &loopinfo) {
    std::list<Loop*> worklist;
    worklist.clear();
    
    LoopInfo::iterator LoopI = loopinfo.begin(), LoopE = loopinfo.end();
    for (;LoopI!=LoopE; LoopI++) {
       Loop * curloop = *LoopI;
       worklist.push_back(curloop);
    }
	while (!worklist.empty()) {
		Loop *thisLoop = worklist.front();
		worklist.pop_front();
		const std::vector<Loop*>&subloops = thisLoop->getSubLoops();
		if (subloops.size() == 0) {
		  innerLoops.push_back(thisLoop);
		} else {//not innermost loop
			for (int i=0, loopsize=subloops.size(); i < loopsize; i++) 
				worklist.push_back(subloops[i]);
		}
	} //end of while empty

}




/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//// Ugly -- the following functions are almost overlapped   ////
///           InsDuplica.cpp                                 ////
/////////////////////////////////////////////////////////////////

bool
RedundAnalysis::duplicable(Value* V) {
	if (Instruction *Ins = dyn_cast<Instruction>(V)) {
		if ( isa<CallInst>(Ins) || isa<TerminatorInst>(Ins) 
				|| isa<StoreInst>(Ins) /*||isa<FreeInst>(Ins) //included in callinst  */
				|| isa<AllocaInst>(Ins) ||  isa<VAArgInst>(Ins))  
			return false;
		else return true;
	} else {
		return isFuncArgu(V);
	}
}

bool 
RedundAnalysis::isFuncArgu(Value *v) {
	if (arguSet.find(v) != arguSet.end()) return true;
	else return false;
}


Instruction* 
RedundAnalysis::findLastCond (BasicBlock *BB) {
    TerminatorInst *lastIns = BB->getTerminator();
    assert(!(isa<SwitchInst>(lastIns)) && "Find a SwitchInst! You need to lower SwitchInst.");
    BranchInst * BI = dyn_cast<BranchInst>(lastIns);
    if (BI && (BI->isConditional())) {
	Value *cond = BI->getCondition();
	Instruction* condIns = dyn_cast<Instruction>(cond); 
	//find condIns. But we have to make sure condIns is the second to the last instruction in BB
	assert(condIns && "Branch Condition must not be trivial");
	if ((condIns->getNextNode())!=lastIns) {
	    //assert((condIns->getParent() == BB) && "condIns is not in the same BB as br!");
	    //if condIns is not in the same BB as br. We have to leave it there
	    if (condIns->getParent() != BB) return lastIns;
	    //if condIns is a PHINode, since we can not move, we just return BI
	    //if condIns has more than one use, better not to reorder
	    if ((isa<PHINode>(condIns))||!(condIns->hasOneUse())) return lastIns;
	    condIns->moveBefore(lastIns); //we moved condInst right before br
	}
    return condIns;
    
    }
    //return the terminator instruction
    return lastIns; 
}


BranchInst *
RedundAnalysis::hasConditionalBr(BasicBlock *BB) {
    TerminatorInst *terminator = BB->getTerminator();
    //if terminator is a branch
    if (BranchInst *BI = dyn_cast<BranchInst>(terminator)) {
	//if it is conditional branch, we will make new blocks to check the branch
	if (BI->isConditional()) return BI;
    }
    return NULL;
}


void
RedundAnalysis::setupFuncArguSet(Function &F) {
    arguSet.clear();
    for (Function::arg_iterator AI = F.arg_begin(), E = F.arg_end();
	 AI != E; ++AI) 
	arguSet.insert(AI);
}
