.LC0:
        .string "Zero."
main:
        pushq   %rax
        movl    $.LC0, %edi
        call    puts
        xorl    %eax, %eax
        popq    %rdx
        ret