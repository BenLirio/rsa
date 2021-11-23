#include <stdio.h>
#include <stdlib.h>
typedef union byte8 {
    unsigned char* foo;
    unsigned long int* bar;
} byte8;


typedef unsigned char byte;
typedef byte dword[8];
typedef dword* bigInt;
int main() {
    byte a = 0;
    dword b;
    b[0] = 3;
    bigInt c = calloc(8, 3);
    //c[1][0] = 1;
    printf("%lx\n", (unsigned long int)c[1]);
}
