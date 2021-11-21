#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef unsigned long int uint64;

typedef struct uint64MulRes {
    uint64 lo;
    uint64 hi;
} uint64MulRes;

typedef struct uint64AddRes {
    uint64 val;
    bool carry;
} uint64AddRes;

typedef char *bigInt;

void printBits(uint64 val) {
    for (int i = 63; i >= 0; i--) {
        if (val&((uint64)1<<i)) {
            printf("1");
        } else {
            printf("0");
        }
    }
}

uint64MulRes mul(uint64 x, uint64 y) {
    uint64 mask32 = ((uint64)1<<32)-1;
    uint64 hi = (x>>32)*(y>>32);
    hi += ((x>>32)*(y&mask32))>>32;
    hi += ((y>>32)*(x&mask32))>>32;
    uint64MulRes z;
    z.lo = x*y;
    z.hi = hi;
    return z;
}

bool hasCarry(uint64 x, uint64 y, bool c) {
    return ((x&y)|((x|y)&((uint64)c<<63)))>>63;
}

uint64AddRes add(uint64 x, uint64 y, bool c) {
    uint64AddRes z;
    z.val = x+y+c;
    z.carry = hasCarry(x, y, c);
    return z;
}


bigInt bigMul(bigInt A, bigInt B) {
    uint64 A0 = 0;
    uint64 B0 = 0;
    uint64 A1 = 0;
    uint64 B1 = 0;
    for (int i = 0; i < 8; i++) {
        A0 |= (uint64)A[i]<<(i*8);
        B0 |= (uint64)B[i]<<(i*8);
        A1 |= (uint64)A[i+8]<<(i*8);
        B1 |= (uint64)B[i+8]<<(i*8);
    }
    uint64MulRes C00 = mul(A0, B0);
    uint64MulRes C01 = mul(A0, B1);
    uint64MulRes C10 = mul(A1, B0);
    uint64MulRes C11 = mul(A1, B1);
    bigInt C = malloc(32);

    uint64AddRes t;
    t.val = 0;
    t.carry = false;
    uint64 w0 = 0;
    uint64 w1 = 0;
    uint64 w2 = 0;
    uint64 w3 = 0;
    uint64 carry = 0;
    w0 = C00.lo;
    t = add(C00.hi, C01.lo, 0);
    carry += t.carry;
    t = add(t.val, C10.lo, 0);
    carry += t.carry;
    w1 = t.val;

    t = add(C01.hi, carry, 0);
    carry = t.carry;
    t = add(t.val, C10.hi, 0);
    carry += t.carry;
    t = add(t.val, C11.lo, 0);
    carry += t.carry;
    w2 = t.val;
    w3 = add(C11.hi, carry, 0).val;
    
    return C;
}

int main() {
    bigInt A = malloc(16);
    bigInt B = malloc(16);
    bigInt C = bigMul(A, B);
    free(A);
    free(B);
    free(C);
}
