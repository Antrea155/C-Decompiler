.LC0:
        .string "%d"
.LC1:
        .string "Enter the number : "
.LC2:
        .string "%ld"
.LC3:
        .string "The armstrong number upto %d are\n"
.LC4:
        .string "%ld\n"
main:
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $48, %rsp
        movl    %edi, -36(%rbp)
        movq    %rsi, -48(%rbp)
        movl    -36(%rbp), %eax
        movl    %eax, %esi
        movl    $.LC0, %edi
        movl    $0, %eax
        call    printf
        movl    $.LC1, %edi
        movl    $0, %eax
        call    printf
        movl    %eax, -12(%rbp)
        leaq    -24(%rbp), %rax
        movq    %rax, %rsi
        movl    $.LC2, %edi
        movl    $0, %eax
        call    __isoc99_scanf
        movl    %eax, -12(%rbp)
        movq    -24(%rbp), %rax
        movq    %rax, %rsi
        movl    $.LC3, %edi
        movl    $0, %eax
        call    printf
        movl    %eax, -12(%rbp)
        movq    $1, -8(%rbp)
        jmp     .L2
.L4:
        movq    -8(%rbp), %rax
        movq    %rax, %rdi
        call    isarmstrong(long)
        testl   %eax, %eax
        setne   %al
        testb   %al, %al
        je      .L3
        movq    -8(%rbp), %rax
        movq    %rax, %rsi
        movl    $.LC4, %edi
        movl    $0, %eax
        call    printf
        movl    %eax, -12(%rbp)
.L3:
        addq    $1, -8(%rbp)
.L2:
        movq    -24(%rbp), %rax
        cmpq    %rax, -8(%rbp)
        jle     .L4
        movl    $0, %eax
        leave
        ret
.LC5:
        .string "ok"
dummy():
        pushq   %rbp
        movq    %rsp, %rbp
        movl    $.LC5, %edi
        movl    $0, %eax
        call    printf
        nop
        popq    %rbp
        ret
isarmstrong(long):
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $64, %rsp
        movq    %rdi, -56(%rbp)
        movl    $10, -36(%rbp)
        movq    $0, -16(%rbp)
        movq    -16(%rbp), %rax
        movq    %rax, -8(%rbp)
        movq    -56(%rbp), %rax
        movq    %rax, -32(%rbp)
        movq    -32(%rbp), %rax
        movq    %rax, -24(%rbp)
        jmp     .L8
.L9:
        movl    -36(%rbp), %eax
        movslq  %eax, %rcx
        movq    -56(%rbp), %rax
        cqto
        idivq   %rcx
        movq    %rax, -56(%rbp)
        addq    $1, -8(%rbp)
.L8:
        cmpq    $0, -56(%rbp)
        jne     .L9
        jmp     .L10
.L11:
        movq    -8(%rbp), %rax
        movl    %eax, %esi
        movl    -36(%rbp), %eax
        movslq  %eax, %rcx
        movq    -24(%rbp), %rax
        cqto
        idivq   %rcx
        movq    %rdx, %rax
        movl    %eax, %edi
        call    power(int, int)
        cltq
        addq    %rax, -16(%rbp)
        movl    -36(%rbp), %eax
        movslq  %eax, %rdi
        movq    -24(%rbp), %rax
        cqto
        idivq   %rdi
        movq    %rax, -24(%rbp)
.L10:
        cmpq    $0, -24(%rbp)
        jne     .L11
        movq    -32(%rbp), %rax
        cmpq    -16(%rbp), %rax
        jne     .L12
        movl    $1, %eax
        jmp     .L13
.L12:
        movl    $0, %eax
.L13:
        leave
        ret
isprime(long):
        pushq   %rbp
        movq    %rsp, %rbp
        movq    %rdi, -24(%rbp)
        movl    $1, -12(%rbp)
        movq    $2, -8(%rbp)
        jmp     .L15
.L17:
        movq    -24(%rbp), %rax
        cqto
        idivq   -8(%rbp)
        movq    %rdx, %rax
        testq   %rax, %rax
        jne     .L16
        movl    $0, -12(%rbp)
.L16:
        addq    $2, -8(%rbp)
.L15:
        movq    -8(%rbp), %rax
        imulq   %rax, %rax
        cmpq    %rax, -24(%rbp)
        jge     .L17
        movl    -12(%rbp), %eax
        popq    %rbp
        ret
power(int, int):
        pushq   %rbp
        movq    %rsp, %rbp
        movl    %edi, -20(%rbp)
        movl    %esi, -24(%rbp)
        movl    $1, -4(%rbp)
        movl    $1, -8(%rbp)
        jmp     .L20
.L21:
        movl    -4(%rbp), %eax
        imull   -20(%rbp), %eax
        movl    %eax, -4(%rbp)
        addl    $1, -8(%rbp)
.L20:
        movl    -8(%rbp), %eax
        cmpl    -24(%rbp), %eax
        jle     .L21
        movl    -4(%rbp), %eax
        popq    %rbp
        ret