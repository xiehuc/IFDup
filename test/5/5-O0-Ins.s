	.file	"5-O0-Ins.bc"
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
	subq	$80, %rsp
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
	movl	-56(%rbp), %eax
	cmpl	$10, %eax
	movl	%eax, -60(%rbp)         # 4-byte Spill
	jge	.LBB0_10
# BB#2:                                 # %_T_
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	-60(%rbp), %eax         # 4-byte Reload
	cmpl	$10, %eax
	jge	.LBB0_12
# BB#3:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	-56(%rbp), %eax
	movslq	%eax, %rcx
	movl	-48(%rbp,%rcx,4), %eax
	movl	-52(%rbp), %edx
	cmpl	%edx, %eax
	movl	%eax, -64(%rbp)         # 4-byte Spill
	movl	%edx, -68(%rbp)         # 4-byte Spill
	jne	.LBB0_6
# BB#4:                                 # %_T_1
	movl	-64(%rbp), %eax         # 4-byte Reload
	movl	-68(%rbp), %ecx         # 4-byte Reload
	cmpl	%ecx, %eax
	jne	.LBB0_12
# BB#5:
	jmp	.LBB0_11
.LBB0_6:                                # %_F_2
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	-64(%rbp), %eax         # 4-byte Reload
	movl	-68(%rbp), %ecx         # 4-byte Reload
	cmpl	%ecx, %eax
	je	.LBB0_12
# BB#7:                                 #   in Loop: Header=BB0_1 Depth=1
	jmp	.LBB0_8
.LBB0_8:                                #   in Loop: Header=BB0_1 Depth=1
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	addl	$1, %ecx
	addl	$1, %eax
	cmpl	%ecx, %eax
	movl	%eax, -72(%rbp)         # 4-byte Spill
	jne	.LBB0_12
# BB#9:                                 # %A3
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	-72(%rbp), %eax         # 4-byte Reload
	movl	%eax, -56(%rbp)
	jmp	.LBB0_1
.LBB0_10:                               # %_F_
	movl	-60(%rbp), %eax         # 4-byte Reload
	cmpl	$10, %eax
	jl	.LBB0_12
.LBB0_11:
	movl	$0, %eax
	addq	$80, %rsp
	popq	%rbp
	ret
.LBB0_12:                               # %main_Error
	movl	$4294967273, %edi       # imm = 0xFFFFFFE9
	callq	exit
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
