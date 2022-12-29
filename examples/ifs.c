#include <stdio.h>


int main() {
    
    int x=3;
    int y=4;

    if (x>y) printf("then if 1");

    if (x>y) {
        if (y>20) printf("nested if");
    }
    if (x>y) printf("greater");
    else printf("smaller");
}