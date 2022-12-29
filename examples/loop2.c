#include <stdio.h>
int func(int x, char y)
{
    char c;
    short j;
    j=10;
    c='a';
    int z =x*7+3;
   unsigned short i;
    for (i = 0; i < 100; ++i) {
        printf("%d\n",i);
    }
    return 0;
}

int main() {
    printf("starting");
    int x=1;
    char c='i';
    func(x,c);
}