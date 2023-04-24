.LC0:
        .string "yes"
main:
        leal    4(%esp), %ecx
        andl    $-16, %esp
        pushl   -4(%ecx)
        pushl   %ebp
        movl    %esp, %ebp
        pushl   %ecx
        subl    $20, %esp
        movl    $0, -12(%ebp)
        movl    $1, -16(%ebp)
        movl    -16(%ebp), %eax
        addl    $2, %eax
        movl    %eax, -12(%ebp)
        cmpl    $3, -12(%ebp)
        jne     .L2
        subl    $12, %esp
        pushl   $.LC0
        call    printf
        addl    $16, %esp
        movl    %eax, -20(%ebp)
.L2:
        movl    $0, %eax
        movl    -4(%ebp), %ecx
        leave
        leal    -4(%ecx), %esp
        ret