	.file	"5-O0.bc"
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
	subq	$64, %rsp
	leaq	.Lmain.a, %rax
	movabsq	$40, %rdx
	leaq	-48(%rbp), %rcx
	movl	$0, -4(%rbp)
	movq	%rcx, %rdi
	movq	%rax, %rsi
	callq	memcpy
	movl	$5, -52(%rbp)
	movl	$0, -56(%rbp)
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	cmpl	$10, -56(%rbp)
	jge	.LBB0_6
# BB#2:                                 #   in Loop: Header=BB0_1 Depth=1
	movslq	-56(%rbp), %rax
	movl	-48(%rbp,%rax,4), %ecx
	cmpl	-52(%rbp), %ecx
	jne	.LBB0_4
# BB#3:
	jmp	.LBB0_6
.LBB0_4:                                #   in Loop: Header=BB0_1 Depth=1
	jmp	.LBB0_5
.LBB0_5:                                #   in Loop: Header=BB0_1 Depth=1
	movl	-56(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -56(%rbp)
	jmp	.LBB0_1
.LBB0_6:
	movl	$0, %eax
	addq	$64, %rsp
	popq	%rbp
	ret
.Ltmp5:
	.size	main, .Ltmp5-main
	.cfi_endproc

	.type	.Lmain.a,@object        # @main.a
	.section	.rodata.str4.16,"aMS",@progbits,4
	.align	16
.Lmain.a:
	.long	3                       # 0x3
	.long	2                       # 0x2
	.long	4                       # 0x4
	.long	6                       # 0x6
	.long	8                       # 0x8
	.long	1                       # 0x1
	.long	5                       # 0x5
	.long	7                       # 0x7
	.long	9                       # 0x9
	.long	0                       # 0x0
	.size	.Lmain.a, 40


	.ident	"Ubuntu clang version 3.4-1ubuntu3 (tags/RELEASE_34/final) (based on LLVM 3.4)"
	.section	".note.GNU-stack","",@progbits
