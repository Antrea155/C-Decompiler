.LC0:
        .string "then if 1"
.LC1:
        .string "nested if"
.LC2:
        .string "greater"
.LC3:
        .string "smaller"
main:
        leal    4(%esp), %ecx
        andl    $-16, %esp
        pushl   -4(%ecx)
        pushl   %ebp
        movl    %esp, %ebp
        pushl   %ecx
        subl    $20, %esp
        movl    $3, -12(%ebp)
        movl    $4, -16(%ebp)
        movl    -12(%ebp), %eax
        cmpl    -16(%ebp), %eax
        jle     .L2
        subl    $12, %esp
        pushl   $.LC0
        call    printf
        addl    $16, %esp
        movl    %eax, -20(%ebp)
.L2:
        movl    -12(%ebp), %eax
        cmpl    -16(%ebp), %eax
        jle     .L3
        cmpl    $20, -16(%ebp)
        jle     .L3
        subl    $12, %esp
        pushl   $.LC1
        call    printf
        addl    $16, %esp
        movl    %eax, -20(%ebp)
.L3:
        movl    -12(%ebp), %eax
        cmpl    -16(%ebp), %eax
        jle     .L4
        subl    $12, %esp
        pushl   $.LC2
        call    printf
        addl    $16, %esp
        movl    %eax, -20(%ebp)
        jmp     .L5
.L4:
        subl    $12, %esp
        pushl   $.LC3
        call    printf
        addl    $16, %esp
        movl    %eax, -20(%ebp)
.L5:
        movl    $0, %eax
        movl    -4(%ebp), %ecx
        leave
        leal    -4(%ecx), %esp
        ret