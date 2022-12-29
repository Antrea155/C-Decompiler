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