power:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	movl	$1, -4(%ebp)
	movl	$1, -8(%ebp)
.L22:
	movl	-8(%ebp), %eax
	cmpl	12(%ebp), %eax
	jle	.L25
	jmp	.L23
.L25:
	movl	-4(%ebp), %eax
	imull	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	leal	-8(%ebp), %eax
	incl	(%eax)
	jmp	.L22
.L23:
	movl	-4(%ebp), %eax
	leave
	ret