.LC0:
        .string "then if 1"
.LC1:
        .string "nested if"
.LC2:
        .string "greater"
.LC3:
        .string "smaller"
main:
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $16, %rsp
        movl    $3, -4(%rbp)
        movl    $4, -8(%rbp)
        movl    -4(%rbp), %eax
        cmpl    -8(%rbp), %eax
        jle     .L2
        movl    $.LC0, %edi
        movl    $0, %eax
        call    printf
        movl    %eax, -12(%rbp)
.L2:
        movl    -4(%rbp), %eax
        cmpl    -8(%rbp), %eax
        jle     .L3
        cmpl    $20, -8(%rbp)
        jle     .L3
        movl    $.LC1, %edi
        movl    $0, %eax
        call    printf
        movl    %eax, -12(%rbp)
.L3:
        movl    -4(%rbp), %eax
        cmpl    -8(%rbp), %eax
        jle     .L4
        movl    $.LC2, %edi
        movl    $0, %eax
        call    printf
        movl    %eax, -12(%rbp)
        jmp     .L5
.L4:
        movl    $.LC3, %edi
        movl    $0, %eax
        call    printf
        movl    %eax, -12(%rbp)
.L5:
        movl    $0, %eax
        leave
        ret