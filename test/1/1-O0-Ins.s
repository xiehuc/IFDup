	.file	"1-O0-Ins.bc"
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
	subq	$32, %rsp
	movl	$0, -4(%rbp)
	movl	$5, -8(%rbp)
	movl	$7, -12(%rbp)
	movl	-8(%rbp), %eax
	movl	-12(%rbp), %ecx
	movl	%eax, %edx
	addl	%ecx, %edx
	addl	%ecx, %eax
	cmpl	%edx, %eax
	movl	%eax, -20(%rbp)         # 4-byte Spill
	jne	.LBB0_2
# BB#1:                                 # %A1
	leaq	.L.str, %rdi
	movl	-20(%rbp), %eax         # 4-byte Reload
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %esi
	movb	$0, %al
	callq	printf
	movl	$0, %esi
	movl	%eax, -24(%rbp)         # 4-byte Spill
	movl	%esi, %eax
	addq	$32, %rsp
	popq	%rbp
	ret
.LBB0_2:                                # %main_Error
	movl	$4294967273, %edi       # imm = 0xFFFFFFE9
	callq	exit
.Ltmp5:
	.size	main, .Ltmp5-main
	.cfi_endproc

	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"%d\n"
	.size	.L.str, 4


	.ident	"Ubuntu clang version 3.4-1ubuntu3 (tags/RELEASE_34/final) (based on LLVM 3.4)"
	.section	".note.GNU-stack","",@progbits
