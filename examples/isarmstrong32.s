isarmstrong:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$10, -20(%ebp)
	movl	$0, -8(%ebp)
	movl	$0, -4(%ebp)
	movl	8(%ebp), %eax
	movl	%eax, -16(%ebp)
	movl	%eax, -12(%ebp)
.L8:
	cmpl	$0, 8(%ebp)
	jne	.L10
	jmp	.L9
.L10:
	movl	-20(%ebp), %edx
	movl	8(%ebp), %eax
	movl	%edx, %ecx
	cltd
	idivl	%ecx
	movl	%eax, 8(%ebp)
	leal	-4(%ebp), %eax
	incl	(%eax)
	jmp	.L8
.L9:
	nop
.L11:
	cmpl	$0, -12(%ebp)
	jne	.L13
	jmp	.L12
.L13:
	subl	$8, %esp
	pushl	-4(%ebp)
	movl	-20(%ebp), %edx
	movl	-12(%ebp), %eax
	movl	%edx, %ecx
	cltd
	idivl	%ecx
	pushl	%edx
	call	power
	addl	$16, %esp
	movl	%eax, %edx
	leal	-8(%ebp), %eax
	addl	%edx, (%eax)
	movl	-20(%ebp), %edx
	movl	-12(%ebp), %eax
	movl	%edx, %ecx
	cltd
	idivl	%ecx
	movl	%eax, -12(%ebp)
	jmp	.L11
.L12:
	movl	-16(%ebp), %eax
	cmpl	-8(%ebp), %eax
	jne	.L14
	movl	$1, -24(%ebp)
	jmp	.L7
.L14:
	movl	$0, -24(%ebp)
.L7:
	movl	-24(%ebp), %eax
	leave
	ret