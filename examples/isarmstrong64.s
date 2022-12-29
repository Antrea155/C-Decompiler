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