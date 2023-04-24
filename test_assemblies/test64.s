.LC0:
        .string "yes"
main:
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $16, %rsp
        movl    $0, -4(%rbp)
        movl    $1, -8(%rbp)
        movl    -8(%rbp), %eax
        addl    $2, %eax
        movl    %eax, -4(%rbp)
        cmpl    $3, -4(%rbp)
        jne     .L2
        movl    $.LC0, %edi
        movl    $0, %eax
        call    printf
        movl    %eax, -12(%rbp)
.L2:
        movl    $0, %eax
        leave
        ret