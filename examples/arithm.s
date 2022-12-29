arith(int, int, int):
        pushq   %rbp
        movq    %rsp, %rbp
        movl    %edi, -36(%rbp)
        movl    %esi, -40(%rbp)
        movl    %edx, -44(%rbp)
        movl    -36(%rbp), %edx
        movl    -40(%rbp), %eax
        addl    %edx, %eax
        movl    %eax, -4(%rbp)
        movl    -44(%rbp), %edx
        movl    -4(%rbp), %eax
        addl    %edx, %eax
        movl    %eax, -8(%rbp)
        movl    -36(%rbp), %eax
        addl    $4, %eax
        movl    %eax, -12(%rbp)
        movl    -40(%rbp), %edx
        movl    %edx, %eax
        addl    %eax, %eax
        addl    %edx, %eax
        sall    $4, %eax
        movl    %eax, -16(%rbp)
        movl    -12(%rbp), %edx
        movl    -16(%rbp), %eax
        addl    %edx, %eax
        movl    %eax, -20(%rbp)
        movl    -8(%rbp), %eax
        imull   -20(%rbp), %eax
        movl    %eax, -24(%rbp)
        movl    -24(%rbp), %eax
        popq    %rbp
        ret