.LC0:
        .string "%d\n"
func(int, char):
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $32, %rsp
        movl    %edi, -20(%rbp)
        movl    %esi, %eax
        movb    %al, -24(%rbp)
        movw    $10, -4(%rbp)
        movb    $97, -5(%rbp)
        movl    -20(%rbp), %edx
        movl    %edx, %eax
        sall    $3, %eax
        subl    %edx, %eax
        addl    $3, %eax
        movl    %eax, -12(%rbp)
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
        cmpw    $99, -2(%rbp)
        jbe     .L3
        movl    $0, %eax
        leave
        ret
.LC1:
        .string "starting"
main:
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $16, %rsp
        movl    $.LC1, %edi
        movl    $0, %eax
        call    printf
        movl    $1, -4(%rbp)
        movb    $105, -5(%rbp)
        movsbl  -5(%rbp), %edx
        movl    -4(%rbp), %eax
        movl    %edx, %esi
        movl    %eax, %edi
        call    func(int, char)
        movl    $0, %eax
        leave
        ret