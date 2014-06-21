	.file	"4-O0-Ins.bc"
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
	subq	$448, %rsp              # imm = 0x1C0
	movl	$0, %esi
	movabsq	$400, %rdx              # imm = 0x190
	leaq	-416(%rbp), %rax
	movl	$0, -4(%rbp)
	movq	%rax, %rdi
	callq	memset
	movl	$0, -420(%rbp)
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movl	-420(%rbp), %eax
	cmpl	$100, %eax
	movl	%eax, -424(%rbp)        # 4-byte Spill
	jge	.LBB0_8
# BB#2:                                 # %_T_
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	-424(%rbp), %eax        # 4-byte Reload
	cmpl	$100, %eax
	jge	.LBB0_10
# BB#3:                                 #   in Loop: Header=BB0_1 Depth=1
	leaq	-416(%rbp), %rax
	movl	-420(%rbp), %ecx
	movslq	%ecx, %rdx
	movl	-416(%rbp,%rdx,4), %ecx
	movl	%ecx, %esi
	addl	$1, %esi
	addl	$1, %ecx
	movl	-420(%rbp), %edi
	movslq	%edi, %rdx
	movslq	%edi, %r8
	shlq	$2, %rdx
	movq	%rax, %r9
	addq	%rdx, %r9
	shlq	$2, %r8
	addq	%r8, %rax
	cmpl	%esi, %ecx
	movq	%rax, -432(%rbp)        # 8-byte Spill
	movq	%r9, -440(%rbp)         # 8-byte Spill
	movl	%ecx, -444(%rbp)        # 4-byte Spill
	jne	.LBB0_10
# BB#4:                                 # %A1
                                        #   in Loop: Header=BB0_1 Depth=1
	movq	-432(%rbp), %rax        # 8-byte Reload
	movq	-440(%rbp), %rcx        # 8-byte Reload
	cmpq	%rcx, %rax
	jne	.LBB0_10
# BB#5:                                 # %A1A
                                        #   in Loop: Header=BB0_1 Depth=1
	movq	-432(%rbp), %rax        # 8-byte Reload
	movl	-444(%rbp), %ecx        # 4-byte Reload
	movl	%ecx, (%rax)
# BB#6:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	-420(%rbp), %eax
	movl	%eax, %ecx
	addl	$1, %ecx
	addl	$1, %eax
	cmpl	%ecx, %eax
	movl	%eax, -448(%rbp)        # 4-byte Spill
	jne	.LBB0_10
# BB#7:                                 # %A4
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	-448(%rbp), %eax        # 4-byte Reload
	movl	%eax, -420(%rbp)
	jmp	.LBB0_1
.LBB0_8:                                # %_F_
	movl	-424(%rbp), %eax        # 4-byte Reload
	cmpl	$100, %eax
	jl	.LBB0_10
# BB#9:
	movl	$0, %eax
	addq	$448, %rsp              # imm = 0x1C0
	popq	%rbp
	ret
.LBB0_10:                               # %main_Error
	movl	$4294967273, %edi       # imm = 0xFFFFFFE9
	callq	exit
.Ltmp5:
	.size	main, .Ltmp5-main
	.cfi_endproc


	.ident	"Ubuntu clang version 3.4-1ubuntu3 (tags/RELEASE_34/final) (based on LLVM 3.4)"
	.section	".note.GNU-stack","",@progbits
