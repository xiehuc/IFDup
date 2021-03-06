#ifndef PRED_DEBUG_H_H
#define PRED_DEBUG_H_H
//**********************************************
//  Must Include this File After All #include
//**********************************************

#include <llvm/Support/raw_ostream.h>

#ifdef ENABLE_DEBUG
#undef DEBUG
#define DEBUG(expr) expr
#undef NDEBUG
#else
#undef DEBUG
#define DEBUG(expr) 
#undef NDEBUG
#define NDEBUG
#endif

#include <assert.h> // NDEBUG macro would affact this

//disable some output code 
//dont use comment because consider code may used in future
//but comment may be delete sometimes
#define DISABLE(expr) 

// a assert with output llvm values
#define Assert(expr, value) assert( (expr) || (errs()<<"\n>>>"<<value<<"<<<\n",0) )

// a assert which always need check
#define AssertRuntime(expr) if( !(expr) ){ \
   errs()<<"Assert Failed:"<<__FILE__<<":"<<__LINE__<<"\n"; \
   assert(0);\
   exit(-1);\
}

#define ret_on_failed(expr,msg,ret) { if(!(expr)){errs()<<"Failed at "<<__LINE__<<":"<<msg<<"\n"; return ret;} }
#define ret_null_fail(expr,msg) ret_on_failed(expr,msg,NULL);

// ==========================================
//                 Duplicated
// ==========================================

#define VERBOSE(expr,verb) ( (expr) || (errs()<<"<<HERE>>:"<<*verb<<"\n",0))

#endif
