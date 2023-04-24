.LC0:
        .string "test."
.LC1:
        .string "x %d"
main:
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $16, %rsp
        movl    $.LC0, %edi
        call    puts
        movl    $45, -4(%rbp)
        subl    $2, -4(%rbp)
        movl    -4(%rbp), %eax
        movl    %eax, %esi
        movl    $.LC1, %edi
        movl    $0, %eax
        call    printf
        movl    $0, %eax
        leave
        ret