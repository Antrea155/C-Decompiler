#include <stdio.h>


int main() {
    
    int x=3;
    int y=4;
    int ret;

    if (x>y) ret=printf("then if 1");

    if (x>y) {
        if (y>20) ret=printf("nested if");
    }
    if (x>y) ret=printf("greater");
    else ret=printf("smaller");
}