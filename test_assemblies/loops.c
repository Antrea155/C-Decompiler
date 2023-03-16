#include <stdio.h>
int main(void)
{
   int ret;
   unsigned short i;
    for (i = 0; i < 1000; ++i) {
        ret = printf("%d\n",i);
    }

    while (i<100) {
        for (int j=1; j<i; j++)
          ret = printf("number is %d %d\n",i,j);
     i++;
    }
    return 0;
}