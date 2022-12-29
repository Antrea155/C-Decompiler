.LC0:
        .string "%d\n"
main:
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $16, %rsp
        movw    $10, -4(%rbp)
        movb    $97, -5(%rbp)
        movw    $0, -2(%rbp)
        jmp     .L2
.L3:
        movzwl  -2(%rbp), %eax
        movl    %eax, %esi
        movl    $.LC0, %edi
        movl    $0, %eax
        call    printf
        addw    $1, -2(%rbp)
.L2:
        cmpw    $999, -2(%rbp)
        jbe     .L3
        movl    $0, %eax
        leave
        ret
        