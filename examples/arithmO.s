arith(int, int, int):
        leal    (%rsi,%rsi,2), %eax
        sall    $4, %eax
        leal    4(%rdi,%rax), %eax
        addl    %esi, %edi
        addl    %edx, %edi
        imull   %edi, %eax
        ret