//-----Developed by Jing Yu -------------//
// stld.h                                //
//=======================================//
//Duplicate all instructions             //
//check store by load again              //
//future work: implement GSR             //
//=======================================//

#ifndef STLD_H
#define STLD_H

#include "InsDuplica.h"

using namespace llvm;

namespace llvm {

    class InsDupStld: public InsDuplica {
    protected:
	virtual BasicBlock* newCheckerStore(Instruction *, BasicBlock *, Instruction* &nextI);
    };
}



#endif //STLD_H
