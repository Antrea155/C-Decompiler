.LC0:
        .string "%d\n"
.LC1:
        .string "number is %d %d\n"
main:
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $16, %rsp
        movw    $0, -2(%rbp)
        jmp     .L2
.L3:
        movzwl  -2(%rbp), %eax
        movl    %eax, %esi
        movl    $.LC0, %edi
        movl    $0, %eax
        call    printf
        movl    %eax, -12(%rbp)
        addw    $1, -2(%rbp)
.L2:
        cmpw    $999, -2(%rbp)
        jbe     .L3
        jmp     .L4
.L7:
        movl    $1, -8(%rbp)
        jmp     .L5
.L6:
        movzwl  -2(%rbp), %eax
        movl    -8(%rbp), %edx
        movl    %eax, %esi
        movl    $.LC1, %edi
        movl    $0, %eax
        call    printf
        movl    %eax, -12(%rbp)
        addl    $1, -8(%rbp)
.L5:
        movzwl  -2(%rbp), %eax
        cmpl    %eax, -8(%rbp)
        jl      .L6
        movzwl  -2(%rbp), %eax
        addl    $1, %eax
        movw    %ax, -2(%rbp)
.L4:
        cmpw    $99, -2(%rbp)
        jbe     .L7
        movl    $0, %eax
        leave
        ret