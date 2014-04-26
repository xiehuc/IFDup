=================================
Transplant From LLVM 1.6 to 3.4
=================================

Transplant DominatorSet to DominatorTree
==========================================

DominatorSet::dominates(BasicBlock* A, BasicBlock* B)
-------------------------------------------------------

返回True，当A dominates B,可以安全的改写为DominatorTree::dominates

before::

	dominatorset->dominates(A,B);

after::

	DominatorTree& DT = getAnalysis<DominatorTree>();
	DT.dominates(A,B);

DominatorSet::isReachable(BasicBlock* BB)
---------------------------------------------

返回True，当BB可到达，如果BB可到达，我们就有关于它的dominator set信息

可以安全的改写为DominatorTree::isReachableFromEntry(BasicBlock* BB),表示的含义是：如果该BB被所在函数的entry入口支配，则返回True。其实现原理是类似的，如果找到了关于BB的信息，则说明它是可到达的。

PostDominatorSet::find(BasicBlock* BB)
----------------------------------------

可以安全的替换为PostDominatorTree::operator[]或者是PostDominatorTree::getNode(BasicBlock*)

PostDominatorSet::dominates(Instruction*,Instruction*)
-------------------------------------------------------

**不能** 安全的替换为 PostDominatorTree::dominates(BasicBlock*, BasicBlock*)
临时写法

after::

	PDT.dominates(Def->getParent(),Use->getParent());

Transplant SetCC to CmpInst
=============================

SetCC 表示对第一类型进行比较，然后返回 i1 的 true or false。
这里需要根据类型分别展开成CMPInst和聚合阶段。
展开CMPInst的示例代码可以参见::

   if(ValuetoCheck->getType()->isIntOrIntVectorTy()||ty->isPointerTy()) /* newSetEQ */
      newSetEQ = new ICmpInst(synchI, ICmpInst::ICMP_EQ, ValuetoCheck, ValuetoCheckDup,
            ValuetoCheck->getName()+nameTag);
   else
      newSetEQ = new FCmpInst(synchI, FCmpInst::FCMP_OEQ, ValuetoCheck, ValuetoCheckDup,
            ValuetoCheck->getName()+nameTag);

聚合是需要使用extractelement 逐一的将vector的元素抽取出来，并进行与操作。可以参见::

   if(newSetEQ->getType()->isVectorTy()){
      /**
       * since setcc hasbeen removed, we extract value one by one and 'and' them
       * if result is 1 means all value is 1, so it means equal, so we can branch it.
       */
      APInt Idx(32, 0);
      Instruction* elemx = ExtractElementInst::Create(newSetEQ, ConstantInt::get(C, Idx), "elem", BBofSynchI);
      for(int i=1;i<ty->getVectorNumElements();i++){
         ++Idx;
         Value* elemy = ExtractElementInst::Create(newSetEQ, ConstantInt::get(C, Idx), "elem", BBofSynchI);
         elemx = BinaryOperator::CreateAnd(elemx, elemy, "aggregation", BBofSynchI);
      }
      newSetEQ = elemx;
   }

