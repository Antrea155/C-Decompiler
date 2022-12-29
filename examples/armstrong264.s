power(int, unsigned int):
        pushq   %rbp
        movq    %rsp, %rbp
        pushq   %rbx
        subq    $24, %rsp
        movl    %edi, -20(%rbp)
        movl    %esi, -24(%rbp)
        cmpl    $0, -24(%rbp)
        jne     .L2
        movl    $1, %eax
        jmp     .L3
.L2:
        movl    -24(%rbp), %eax
        andl    $1, %eax
        testl   %eax, %eax
        jne     .L4
        movl    -24(%rbp), %eax
        shrl    %eax
        movl    %eax, %edx
        movl    -20(%rbp), %eax
        movl    %edx, %esi
        movl    %eax, %edi
        call    power(int, unsigned int)
        movl    %eax, %ebx
        movl    -24(%rbp), %eax
        shrl    %eax
        movl    %eax, %edx
        movl    -20(%rbp), %eax
        movl    %edx, %esi
        movl    %eax, %edi
        call    power(int, unsigned int)
        imull   %ebx, %eax
        jmp     .L3
.L4:
        movl    -24(%rbp), %eax
        shrl    %eax
        movl    %eax, %edx
        movl    -20(%rbp), %eax
        movl    %edx, %esi
        movl    %eax, %edi
        call    power(int, unsigned int)
        imull   -20(%rbp), %eax
        movl    %eax, %ebx
        movl    -24(%rbp), %eax
        shrl    %eax
        movl    %eax, %edx
        movl    -20(%rbp), %eax
        movl    %edx, %esi
        movl    %eax, %edi
        call    power(int, unsigned int)
        imull   %ebx, %eax
.L3:
        movq    -8(%rbp), %rbx
        leave
        ret
order(int):
        pushq   %rbp
        movq    %rsp, %rbp
        movl    %edi, -20(%rbp)
        movl    $0, -4(%rbp)
        jmp     .L6
.L7:
        addl    $1, -4(%rbp)
        movl    -20(%rbp), %eax
        movslq  %eax, %rdx
        imulq   $1717986919, %rdx, %rdx
        shrq    $32, %rdx
        movl    %edx, %ecx
        sarl    $2, %ecx
        cltd
        movl    %ecx, %eax
        subl    %edx, %eax
        movl    %eax, -20(%rbp)
.L6:
        cmpl    $0, -20(%rbp)
        jne     .L7
        movl    -4(%rbp), %eax
        popq    %rbp
        ret
isArmstrong(int):
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $32, %rsp
        movl    %edi, -20(%rbp)
        movl    -20(%rbp), %eax
        movl    %eax, %edi
        call    order(int)
        movl    %eax, -12(%rbp)
        movl    -20(%rbp), %eax
        movl    %eax, -4(%rbp)
        movl    $0, -8(%rbp)
        jmp     .L10
.L11:
        movl    -4(%rbp), %edx
        movslq  %edx, %rax
        imulq   $1717986919, %rax, %rax
        shrq    $32, %rax
        movl    %eax, %ecx
        sarl    $2, %ecx
        movl    %edx, %eax
        sarl    $31, %eax
        subl    %eax, %ecx
        movl    %ecx, %eax
        sall    $2, %eax
        addl    %ecx, %eax
        addl    %eax, %eax
        subl    %eax, %edx
        movl    %edx, -16(%rbp)
        movl    -12(%rbp), %edx
        movl    -16(%rbp), %eax
        movl    %edx, %esi
        movl    %eax, %edi
        call    power(int, unsigned int)
        addl    %eax, -8(%rbp)
        movl    -4(%rbp), %eax
        movslq  %eax, %rdx
        imulq   $1717986919, %rdx, %rdx
        shrq    $32, %rdx
        movl    %edx, %ecx
        sarl    $2, %ecx
        cltd
        movl    %ecx, %eax
        subl    %edx, %eax
        movl    %eax, -4(%rbp)
.L10:
        cmpl    $0, -4(%rbp)
        jne     .L11
        movl    -8(%rbp), %eax
        cmpl    -20(%rbp), %eax
        jne     .L12
        movl    $1, %eax
        jmp     .L13
.L12:
        movl    $0, %eax
.L13:
        leave
        ret
.LC0:
        .string "True"
.LC1:
        .string "False"
main:
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $16, %rsp
        movl    $153, -4(%rbp)
        movl    -4(%rbp), %eax
        movl    %eax, %edi
        call    isArmstrong(int)
        cmpl    $1, %eax
        sete    %al
        testb   %al, %al
        je      .L15
        movl    $.LC0, %edi
        call    puts
        jmp     .L16
.L15:
        movl    $.LC1, %edi
        call    puts
.L16:
        movl    $1253, -4(%rbp)
        movl    -4(%rbp), %eax
        movl    %eax, %edi
        call    isArmstrong(int)
        cmpl    $1, %eax
        sete    %al
        testb   %al, %al
        je      .L17
        movl    $.LC0, %edi
        call    puts
        jmp     .L18
.L17:
        movl    $.LC1, %edi
        call    puts
.L18:
        movl    $0, %eax
        leave
        ret