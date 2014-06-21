	.file	"4-O0.bc"
	.text
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp2:
	.cfi_def_cfa_offset 16
.Ltmp3:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp4:
	.cfi_def_cfa_register %rbp
	subq	$432, %rsp              # imm = 0x1B0
	movl	$0, %esi
	movabsq	$400, %rdx              # imm = 0x190
	leaq	-416(%rbp), %rax
	movl	$0, -4(%rbp)
	movq	%rax, %rdi
	callq	memset
	movl	$0, -420(%rbp)
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	cmpl	$100, -420(%rbp)
	jge	.LBB0_4
# BB#2:                                 #   in Loop: Header=BB0_1 Depth=1
	movslq	-420(%rbp), %rax
	movl	-416(%rbp,%rax,4), %ecx
	addl	$1, %ecx
	movslq	-420(%rbp), %rax
	movl	%ecx, -416(%rbp,%rax,4)
# BB#3:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	-420(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -420(%rbp)
	jmp	.LBB0_1
.LBB0_4:
	movl	$0, %eax
	addq	$432, %rsp              # imm = 0x1B0
	popq	%rbp
	ret
.Ltmp5:
	.size	main, .Ltmp5-main
	.cfi_endproc


	.ident	"Ubuntu clang version 3.4-1ubuntu3 (tags/RELEASE_34/final) (based on LLVM 3.4)"
	.section	".note.GNU-stack","",@progbits
