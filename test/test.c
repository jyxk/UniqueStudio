#include <stdio.h>


int a;
char b;
int main(void) {
    static int c;
    static char d;
    printf("%d %d %d %d \n", a, b, c, d);
    return 0;
}