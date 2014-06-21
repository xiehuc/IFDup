	.file	"6-O0-Ins.ll"
	.text
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp3:
	.cfi_def_cfa_offset 16
.Ltmp4:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp5:
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	subq	$264, %rsp              # imm = 0x108
.Ltmp6:
	.cfi_offset %rbx, -24
	movl	$0, %esi
	movabsq	$36, %rax
	leaq	-144(%rbp), %rcx
	leaq	.Lmain.b, %rdx
	leaq	-96(%rbp), %rdi
	leaq	.Lmain.a, %r8
	leaq	-48(%rbp), %r9
	movl	$0, -12(%rbp)
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
                                        #     Child Loop BB0_4 Depth 2
                                        #       Child Loop BB0_7 Depth 3
	movl	-148(%rbp), %eax
	cmpl	$3, %eax
	movl	%eax, -204(%rbp)        # 4-byte Spill
	jge	.LBB0_24
# BB#2:                                 # %_T_
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	-204(%rbp), %eax        # 4-byte Reload
	cmpl	$3, %eax
	jge	.LBB0_26
# BB#3:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	$0, -152(%rbp)
.LBB0_4:                                #   Parent Loop BB0_1 Depth=1
                                        # =>  This Loop Header: Depth=2
                                        #       Child Loop BB0_7 Depth 3
	movl	-152(%rbp), %eax
	cmpl	$3, %eax
	movl	%eax, -208(%rbp)        # 4-byte Spill
	jge	.LBB0_20
# BB#5:                                 # %_T_1
                                        #   in Loop: Header=BB0_4 Depth=2
	movl	-208(%rbp), %eax        # 4-byte Reload
	cmpl	$3, %eax
	jge	.LBB0_26
# BB#6:                                 #   in Loop: Header=BB0_4 Depth=2
	movl	$0, -156(%rbp)
.LBB0_7:                                #   Parent Loop BB0_1 Depth=1
                                        #     Parent Loop BB0_4 Depth=2
                                        # =>    This Inner Loop Header: Depth=3
	movl	-156(%rbp), %eax
	cmpl	$3, %eax
	movl	%eax, -212(%rbp)        # 4-byte Spill
	jge	.LBB0_16
# BB#8:                                 # %_T_4
                                        #   in Loop: Header=BB0_7 Depth=3
	movl	-212(%rbp), %eax        # 4-byte Reload
	cmpl	$3, %eax
	jge	.LBB0_26
# BB#9:                                 #   in Loop: Header=BB0_7 Depth=3
	leaq	-48(%rbp), %rax
	movl	-156(%rbp), %ecx
	movslq	%ecx, %rdx
	movslq	%ecx, %rsi
	movl	-148(%rbp), %ecx
	movslq	%ecx, %rdi
	movslq	%ecx, %r8
	imulq	$12, %rdi, %rdi
	movq	%rax, %r9
	addq	%rdi, %r9
	imulq	$12, %r8, %rdi
	addq	%rdi, %rax
	shlq	$2, %rdx
	addq	%rdx, %r9
	shlq	$2, %rsi
	addq	%rsi, %rax
	cmpq	%r9, %rax
	movq	%rax, -224(%rbp)        # 8-byte Spill
	jne	.LBB0_26
# BB#10:                                # %lA8
                                        #   in Loop: Header=BB0_7 Depth=3
	leaq	-96(%rbp), %rax
	movq	-224(%rbp), %rcx        # 8-byte Reload
	movl	(%rcx), %edx
	movl	-152(%rbp), %esi
	movslq	%esi, %rdi
	movslq	%esi, %r8
	movl	-156(%rbp), %esi
	movslq	%esi, %r9
	movslq	%esi, %r10
	imulq	$12, %r9, %r9
	movq	%rax, %r11
	addq	%r9, %r11
	imulq	$12, %r10, %r9
	addq	%r9, %rax
	shlq	$2, %rdi
	addq	%rdi, %r11
	shlq	$2, %r8
	addq	%r8, %rax
	cmpq	%r11, %rax
	movq	%rax, -232(%rbp)        # 8-byte Spill
	movl	%edx, -236(%rbp)        # 4-byte Spill
	jne	.LBB0_26
# BB#11:                                # %lA8lA
                                        #   in Loop: Header=BB0_7 Depth=3
	leaq	-144(%rbp), %rax
	movq	-232(%rbp), %rcx        # 8-byte Reload
	movl	(%rcx), %edx
	movl	-236(%rbp), %esi        # 4-byte Reload
	imull	%edx, %esi
	movl	-236(%rbp), %edi        # 4-byte Reload
	imull	%edx, %edi
	movl	-152(%rbp), %edx
	movslq	%edx, %r8
	movslq	%edx, %r9
	movl	-148(%rbp), %edx
	movslq	%edx, %r10
	movslq	%edx, %r11
	imulq	$12, %r10, %r10
	movq	%rax, %rbx
	addq	%r10, %rbx
	imulq	$12, %r11, %r10
	addq	%r10, %rax
	shlq	$2, %r8
	addq	%r8, %rbx
	shlq	$2, %r9
	addq	%r9, %rax
	cmpq	%rbx, %rax
	movq	%rax, -248(%rbp)        # 8-byte Spill
	movl	%edi, -252(%rbp)        # 4-byte Spill
	movl	%esi, -256(%rbp)        # 4-byte Spill
	jne	.LBB0_26
# BB#12:                                # %lA8lAlA
                                        #   in Loop: Header=BB0_7 Depth=3
	movq	-248(%rbp), %rax        # 8-byte Reload
	movl	(%rax), %ecx
	movl	%ecx, %edx
	movl	-256(%rbp), %esi        # 4-byte Reload
	addl	%esi, %edx
	movl	-252(%rbp), %edi        # 4-byte Reload
	addl	%edi, %ecx
	cmpl	%edx, %ecx
	movl	%ecx, -260(%rbp)        # 4-byte Spill
	jne	.LBB0_26
# BB#13:                                # %lA8lAlAA
                                        #   in Loop: Header=BB0_7 Depth=3
	movq	-248(%rbp), %rax        # 8-byte Reload
	movl	-260(%rbp), %ecx        # 4-byte Reload
	movl	%ecx, (%rax)
# BB#14:                                #   in Loop: Header=BB0_7 Depth=3
	movl	-156(%rbp), %eax
	movl	%eax, %ecx
	addl	$1, %ecx
	addl	$1, %eax
	cmpl	%ecx, %eax
	movl	%eax, -264(%rbp)        # 4-byte Spill
	jne	.LBB0_26
# BB#15:                                # %A13
                                        #   in Loop: Header=BB0_7 Depth=3
	movl	-264(%rbp), %eax        # 4-byte Reload
	movl	%eax, -156(%rbp)
	jmp	.LBB0_7
.LBB0_16:                               # %_F_5
                                        #   in Loop: Header=BB0_4 Depth=2
	movl	-212(%rbp), %eax        # 4-byte Reload
	cmpl	$3, %eax
	jl	.LBB0_26
# BB#17:                                #   in Loop: Header=BB0_4 Depth=2
	jmp	.LBB0_18
.LBB0_18:                               #   in Loop: Header=BB0_4 Depth=2
	movl	-152(%rbp), %eax
	movl	%eax, %ecx
	addl	$1, %ecx
	addl	$1, %eax
	cmpl	%ecx, %eax
	movl	%eax, -268(%rbp)        # 4-byte Spill
	jne	.LBB0_26
# BB#19:                                # %A7
                                        #   in Loop: Header=BB0_4 Depth=2
	movl	-268(%rbp), %eax        # 4-byte Reload
	movl	%eax, -152(%rbp)
	jmp	.LBB0_4
.LBB0_20:                               # %_F_2
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	-208(%rbp), %eax        # 4-byte Reload
	cmpl	$3, %eax
	jl	.LBB0_26
# BB#21:                                #   in Loop: Header=BB0_1 Depth=1
	jmp	.LBB0_22
.LBB0_22:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-148(%rbp), %eax
	movl	%eax, %ecx
	addl	$1, %ecx
	addl	$1, %eax
	cmpl	%ecx, %eax
	movl	%eax, -272(%rbp)        # 4-byte Spill
	jne	.LBB0_26
# BB#23:                                # %A3
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	-272(%rbp), %eax        # 4-byte Reload
	movl	%eax, -148(%rbp)
	jmp	.LBB0_1
.LBB0_24:                               # %_F_
	movl	-204(%rbp), %eax        # 4-byte Reload
	cmpl	$3, %eax
	jl	.LBB0_26
# BB#25:
	movl	$0, %eax
	addq	$264, %rsp              # imm = 0x108
	popq	%rbx
	popq	%rbp
	ret
.LBB0_26:                               # %main_Error
	movl	$4294967273, %edi       # imm = 0xFFFFFFE9
	callq	exit
.Ltmp7:
	.size	main, .Ltmp7-main
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
