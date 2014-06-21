	.file	"6-O0.bc"
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
	subq	$208, %rsp
	movl	$0, %esi
	movabsq	$36, %rax
	leaq	-144(%rbp), %rcx
	leaq	.Lmain.b, %rdx
	leaq	-96(%rbp), %rdi
	leaq	.Lmain.a, %r8
	leaq	-48(%rbp), %r9
	movl	$0, -4(%rbp)
	movq	%rdi, -168(%rbp)        # 8-byte Spill
	movq	%r9, %rdi
	movl	%esi, -172(%rbp)        # 4-byte Spill
	movq	%r8, %rsi
	movq	%rdx, -184(%rbp)        # 8-byte Spill
	movq	%rax, %rdx
	movq	%rcx, -192(%rbp)        # 8-byte Spill
	movq	%rax, -200(%rbp)        # 8-byte Spill
	callq	memcpy
	movq	-168(%rbp), %rax        # 8-byte Reload
	movq	%rax, %rdi
	movq	-184(%rbp), %rsi        # 8-byte Reload
	movq	-200(%rbp), %rdx        # 8-byte Reload
	callq	memcpy
	movq	-192(%rbp), %rax        # 8-byte Reload
	movq	%rax, %rdi
	movl	-172(%rbp), %esi        # 4-byte Reload
	movq	-200(%rbp), %rdx        # 8-byte Reload
	callq	memset
	movl	$0, -148(%rbp)
.LBB0_1:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_3 Depth 2
                                        #       Child Loop BB0_5 Depth 3
	cmpl	$3, -148(%rbp)
	jge	.LBB0_12
# BB#2:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	$0, -152(%rbp)
.LBB0_3:                                #   Parent Loop BB0_1 Depth=1
                                        # =>  This Loop Header: Depth=2
                                        #       Child Loop BB0_5 Depth 3
	cmpl	$3, -152(%rbp)
	jge	.LBB0_10
# BB#4:                                 #   in Loop: Header=BB0_3 Depth=2
	movl	$0, -156(%rbp)
.LBB0_5:                                #   Parent Loop BB0_1 Depth=1
                                        #     Parent Loop BB0_3 Depth=2
                                        # =>    This Inner Loop Header: Depth=3
	cmpl	$3, -156(%rbp)
	jge	.LBB0_8
# BB#6:                                 #   in Loop: Header=BB0_5 Depth=3
	leaq	-144(%rbp), %rax
	leaq	-96(%rbp), %rcx
	leaq	-48(%rbp), %rdx
	movslq	-156(%rbp), %rsi
	movslq	-148(%rbp), %rdi
	imulq	$12, %rdi, %rdi
	addq	%rdi, %rdx
	movl	(%rdx,%rsi,4), %r8d
	movslq	-152(%rbp), %rdx
	movslq	-156(%rbp), %rsi
	imulq	$12, %rsi, %rsi
	addq	%rsi, %rcx
	imull	(%rcx,%rdx,4), %r8d
	movslq	-152(%rbp), %rcx
	movslq	-148(%rbp), %rdx
	imulq	$12, %rdx, %rdx
	addq	%rdx, %rax
	movl	(%rax,%rcx,4), %r9d
	addl	%r8d, %r9d
	movl	%r9d, (%rax,%rcx,4)
# BB#7:                                 #   in Loop: Header=BB0_5 Depth=3
	movl	-156(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -156(%rbp)
	jmp	.LBB0_5
.LBB0_8:                                #   in Loop: Header=BB0_3 Depth=2
	jmp	.LBB0_9
.LBB0_9:                                #   in Loop: Header=BB0_3 Depth=2
	movl	-152(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -152(%rbp)
	jmp	.LBB0_3
.LBB0_10:                               #   in Loop: Header=BB0_1 Depth=1
	jmp	.LBB0_11
.LBB0_11:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-148(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -148(%rbp)
	jmp	.LBB0_1
.LBB0_12:
	movl	$0, %eax
	addq	$208, %rsp
	popq	%rbp
	ret
.Ltmp5:
	.size	main, .Ltmp5-main
	.cfi_endproc

	.type	.Lmain.a,@object        # @main.a
	.section	.rodata,"a",@progbits
	.align	16
.Lmain.a:
	.long	1                       # 0x1
	.long	2                       # 0x2
	.long	3                       # 0x3
	.long	4                       # 0x4
	.long	5                       # 0x5
	.long	6                       # 0x6
	.long	7                       # 0x7
	.long	8                       # 0x8
	.long	9                       # 0x9
	.size	.Lmain.a, 36

	.type	.Lmain.b,@object        # @main.b
	.align	16
.Lmain.b:
	.long	2                       # 0x2
	.long	3                       # 0x3
	.long	4                       # 0x4
	.long	5                       # 0x5
	.long	6                       # 0x6
	.long	7                       # 0x7
	.long	9                       # 0x9
	.long	1                       # 0x1
	.long	8                       # 0x8
	.size	.Lmain.b, 36


	.ident	"Ubuntu clang version 3.4-1ubuntu3 (tags/RELEASE_34/final) (based on LLVM 3.4)"
	.section	".note.GNU-stack","",@progbits
