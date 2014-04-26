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
