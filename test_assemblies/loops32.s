.LC0:
        .string "%d\n"
.LC1:
        .string "number is %d %d\n"
main:
        leal    4(%esp), %ecx
        andl    $-16, %esp
        pushl   -4(%ecx)
        pushl   %ebp
        movl    %esp, %ebp
        pushl   %ecx
        subl    $20, %esp
        movw    $0, -10(%ebp)
        jmp     .L2
.L3:
        movzwl  -10(%ebp), %eax
        subl    $8, %esp
        pushl   %eax
        pushl   $.LC0
        call    printf
        addl    $16, %esp
        movl    %eax, -20(%ebp)
        addw    $1, -10(%ebp)
.L2:
        cmpw    $999, -10(%ebp)
        jbe     .L3
        jmp     .L4
.L7:
        movl    $1, -16(%ebp)
        jmp     .L5
.L6:
        movzwl  -10(%ebp), %eax
        subl    $4, %esp
        pushl   -16(%ebp)
        pushl   %eax
        pushl   $.LC1
        call    printf
        addl    $16, %esp
        movl    %eax, -20(%ebp)
        addl    $1, -16(%ebp)
.L5:
        movzwl  -10(%ebp), %eax
        cmpl    %eax, -16(%ebp)
        jl      .L6
        movzwl  -10(%ebp), %eax
        addl    $1, %eax
        movw    %ax, -10(%ebp)
.L4:
        cmpw    $99, -10(%ebp)
        jbe     .L7
        movl    $0, %eax
        movl    -4(%ebp), %ecx
        leave
        leal    -4(%ecx), %esp
        ret