#include <stdbool.h>
#include <assert.h>
#include "./mod.h"

void printBigInt(bigInt val, int size) {
    for (int i = size-1; i >= 0; i--) {
        printf("%.16lx ", val[i]);
        if (i != size-1 && i%4 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void Set(bigInt dst, bigInt src) {
    for (int i = 0; i < (2048>>6); i++) {
        dst[i] = src[i];
    }
}

void SetUint64(bigInt dst, uint64 val) {
    dst[0] = val;
}

uint64 uint64_mul_hi(uint64 a, uint64 b) {
    // automatically return x0
    asm("umulh x0, x0, x1");
}

uint64 uint64_add(uint64 a, uint64 b, bool* c) {
    asm("adds x0, x0, x1\nadc x10, xzr, xzr\nstr x10, [x2]");
}

bool has_carry(uint64 A, uint64 B) {
    bool rtn = ((A&B)|((A|B)&(~(A+B))))>>63 == 1;
    return rtn;
}

void Mul(bigInt dst, bigInt a, bigInt b) {
    uint64 M[2048>>6][2048>>6][2];
    for (int i = 0; i < (2048>>6); i++) {
        for (int j = 0; j < (2048>>6); j++) {
            M[i][j][0] = a[i]*b[j];
            M[i][j][1] = uint64_mul_hi(a[i], b[j]);
        }
    }
    bigInt tmp = calloc(1, 2*(2048>>3));
    bigInt carry = calloc(1, 2*(2048>>3)+1);
    for (int i = 0; i < (2048>>6); i++) {
        for (int j = 0; j < (2048>>6); j++) {
            for (int k = 0; k < 2; k++) {
                bool c;
                tmp[i+j+k] = uint64_add(tmp[i+j+k], M[i][j][k], &c);
                carry[i+j+k+1] += c;
            }
        }
    }
    for (int i = 0; i < 2*(2048>>6); i++) {
        bool c;
        tmp[i] = uint64_add(tmp[i], carry[i], &c);
        carry[i+1] += c;
    }
    for (int i = 0; i < 2*(2048>>6); i++) {
        dst[i] = tmp[i];
    }
}

void MulMod(bigInt dst, bigInt a, bigInt b, bigInt m) {
    bigInt tmp = calloc(1, 2*(2048>>3));
    Mul(tmp, a, b);
    bigInt b_m = calloc(1, 2*(2048>>3));
    for (int i = 0; i < (2048>>6); i++) {
        b_m[i] = m[i];
    }
    Mod(tmp, tmp, b_m);
    for (int i = 0; i < (2048>>6); i++) {
        dst[i] = tmp[i];
    }
}

void Exp(bigInt dst, bigInt b, bigInt e, bigInt m) {
    bigInt cur = malloc(2048>>3);
    Set(cur, b);
    SetUint64(dst, 1);
    for (int i = 0; i < 2048; i++) {
        int idx = i/64;
        uint64 bit_mask = (uint64)1<<(i%64);
        if (e[idx]&bit_mask) {
            MulMod(dst, dst, cur, m);
        }
        MulMod(cur, cur, cur, m);
    }
    free(cur);
}
