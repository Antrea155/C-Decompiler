power(int, unsigned int):
        pushl   %ebp
        movl    %esp, %ebp
        subl    $24, %esp
        cmpl    $0, 12(%ebp)
        jne     .L2
        movl    $1, %eax
        jmp     .L3
.L2:
        movl    12(%ebp), %eax
        shrl    %eax
        subl    $8, %esp
        pushl   %eax
        pushl   8(%ebp)
        call    power(int, unsigned int)
        addl    $16, %esp
        movl    %eax, -16(%ebp)
        movl    12(%ebp), %eax
        andl    $1, %eax
        testl   %eax, %eax
        jne     .L4
        movl    -16(%ebp), %eax
        imull   %eax, %eax
        movl    %eax, -12(%ebp)
        jmp     .L5
.L4:
        movl    8(%ebp), %eax
        imull   -16(%ebp), %eax
        movl    -16(%ebp), %edx
        imull   %edx, %eax
        movl    %eax, -12(%ebp)
.L5:
        movl    -12(%ebp), %eax
.L3:
        leave
        ret
order(int):
        pushl   %ebp
        movl    %esp, %ebp
        subl    $16, %esp
        movl    $0, -4(%ebp)
        jmp     .L7
.L8:
        addl    $1, -4(%ebp)
        movl    8(%ebp), %ecx
        movl    $1717986919, %edx
        movl    %ecx, %eax
        imull   %edx
        movl    %edx, %eax
        sarl    $2, %eax
        sarl    $31, %ecx
        movl    %ecx, %edx
        subl    %edx, %eax
        movl    %eax, 8(%ebp)
.L7:
        cmpl    $0, 8(%ebp)
        jne     .L8
        movl    -4(%ebp), %eax
        leave
        ret
isArmstrong(long):
        pushl   %ebp
        movl    %esp, %ebp
        pushl   %ebx
        subl    $36, %esp
        movl    $10, -28(%ebp)
        movl    $0, -16(%ebp)
        movl    -16(%ebp), %eax
        movl    %eax, -12(%ebp)
        movl    8(%ebp), %eax
        movl    %eax, -24(%ebp)
        movl    -24(%ebp), %eax
        movl    %eax, -20(%ebp)
        jmp     .L11
.L12:
        movl    -28(%ebp), %ecx
        movl    8(%ebp), %eax
        cltd
        idivl   %ecx
        movl    %eax, 8(%ebp)
        addl    $1, -12(%ebp)
.L11:
        cmpl    $0, 8(%ebp)
        jne     .L12
        jmp     .L13
.L14:
        movl    -12(%ebp), %ecx
        movl    -28(%ebp), %ebx
        movl    -20(%ebp), %eax
        cltd
        idivl   %ebx
        movl    %edx, %eax
        subl    $8, %esp
        pushl   %ecx
        pushl   %eax
        call    power(int, unsigned int)
        addl    $16, %esp
        addl    %eax, -16(%ebp)
        movl    -28(%ebp), %ebx
        movl    -20(%ebp), %eax
        cltd
        idivl   %ebx
        movl    %eax, -20(%ebp)
.L13:
        cmpl    $0, -20(%ebp)
        jne     .L14
        movl    -24(%ebp), %eax
        cmpl    -16(%ebp), %eax
        jne     .L15
        movl    $1, %eax
        jmp     .L16
.L15:
        movl    $0, %eax
.L16:
        movl    -4(%ebp), %ebx
        leave
        ret
.LC0:
        .string "True\n"
.LC1:
        .string "False\n"
main:
        leal    4(%esp), %ecx
        andl    $-16, %esp
        pushl   -4(%ecx)
        pushl   %ebp
        movl    %esp, %ebp
        pushl   %ecx
        subl    $20, %esp
        movl    $153, -12(%ebp)
        subl    $12, %esp
        pushl   -12(%ebp)
        call    isArmstrong(long)
        addl    $16, %esp
        cmpl    $1, %eax
        sete    %al
        testb   %al, %al
        je      .L18
        subl    $12, %esp
        pushl   $.LC0
        call    printf
        addl    $16, %esp
        movl    %eax, -16(%ebp)
        jmp     .L19
.L18:
        subl    $12, %esp
        pushl   $.LC1
        call    printf
        addl    $16, %esp
        movl    %eax, -16(%ebp)
.L19:
        movl    $1253, -12(%ebp)
        subl    $12, %esp
        pushl   -12(%ebp)
        call    isArmstrong(long)
        addl    $16, %esp
        cmpl    $1, %eax
        sete    %al
        testb   %al, %al
        je      .L20
        subl    $12, %esp
        pushl   $.LC0
        call    printf
        addl    $16, %esp
        movl    %eax, -16(%ebp)
        jmp     .L21
.L20:
        subl    $12, %esp
        pushl   $.LC1
        call    printf
        addl    $16, %esp
        movl    %eax, -16(%ebp)
.L21:
        movl    $0, %eax
        movl    -4(%ebp), %ecx
        leave
        leal    -4(%ecx), %esp
        ret