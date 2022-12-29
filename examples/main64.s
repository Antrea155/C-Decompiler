.LC0:
        .string "%d"
.LC1:
        .string "Enter the number : "
.LC2:
        .string "%ld"
.LC3:
        .string "The armstrong number upto %d are\n"
.LC4:
        .string "%ld\n"
main:
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $48, %rsp
        movl    %edi, -36(%rbp)
        movq    %rsi, -48(%rbp)
        movl    -36(%rbp), %eax
        movl    %eax, %esi
        movl    $.LC0, %edi
        movl    $0, %eax
        call    printf
        movl    $.LC1, %edi
        movl    $0, %eax
        call    printf
        movl    %eax, -12(%rbp)
        leaq    -24(%rbp), %rax
        movq    %rax, %rsi
        movl    $.LC2, %edi
        movl    $0, %eax
        call    __isoc99_scanf
        movl    %eax, -12(%rbp)
        movq    -24(%rbp), %rax
        movq    %rax, %rsi
        movl    $.LC3, %edi
        movl    $0, %eax
        call    printf
        movl    %eax, -12(%rbp)
        movq    $1, -8(%rbp)
        jmp     .L2
.L4:
        movq    -8(%rbp), %rax
        movq    %rax, %rdi
        call    isarmstrong(long)
        testl   %eax, %eax
        setne   %al
        testb   %al, %al
        je      .L3
        movq    -8(%rbp), %rax
        movq    %rax, %rsi
        movl    $.LC4, %edi
        movl    $0, %eax
        call    printf
        movl    %eax, -12(%rbp)
.L3:
        addq    $1, -8(%rbp)
.L2:
        movq    -24(%rbp), %rax
        cmpq    %rax, -8(%rbp)
        jle     .L4
        movl    $0, %eax
        leave
        ret
.LC5:
        .string "ok"