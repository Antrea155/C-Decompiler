.LC0:
	.string "Enter the number : "
.LC1:
	.string "%ld"
.LC2:
	.string "The armstrong number upto %d are\n"
.LC3:
	.string "%ld\n"
main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	andl	$-16, %esp
	movl	$0, %eax
	subl	%eax, %esp
	subl	$12, %esp
	pushl	$.LC0
	call	printf
	addl	$16, %esp
	movl	%eax, -12(%ebp)
	subl	$8, %esp
	leal	-4(%ebp), %eax
	pushl	%eax
	pushl	$.LC1
	call	scanf
	addl	$16, %esp
	movl	%eax, -12(%ebp)
	subl	$8, %esp
	pushl	-4(%ebp)
	pushl	$.LC2
	call	printf
	addl	$16, %esp
	movl	%eax, -12(%ebp)
	movl	$1, -8(%ebp)
.L2:
	movl	-8(%ebp), %eax
	cmpl	-4(%ebp), %eax
	jle	.L5
	jmp	.L3
.L5:
	subl	$12, %esp
	pushl	-8(%ebp)
	call	isarmstrong
	addl	$16, %esp
	testl	%eax, %eax
	je	.L4
	subl	$8, %esp
	pushl	-8(%ebp)
	pushl	$.LC3
	call	printf
	addl	$16, %esp
	movl	%eax, -12(%ebp)
.L4:
	leal	-8(%ebp), %eax
	incl	(%eax)
	jmp	.L2
.L3:
	movl	$0, %eax
	leave
	ret