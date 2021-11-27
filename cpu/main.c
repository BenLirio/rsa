#include <fcntl.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned long int uint64;

typedef struct bigInt {
    uint64* data;
    int len;
} bigInt;

void panic(char* msg) {
    printf("%s\n", msg);
    exit(1);
}

uint64 mul_lo(uint64 A, uint64 B) {
    return A*B;
}

uint64 mul_hi(uint64 A, uint64 B) {
    /*
    __asm__(
            "umulh x0, x0, x1"
   );
    register uint64 res asm("x0");
    return res;
    */
    uint64 x = A;
    uint64 y = B;
    uint64 x0 = (x<<32)>>32;
    uint64 x1 = x>>32;
    uint64 y0 = (y<<32)>>32;
    uint64 y1 = y>>32;
    uint64 w0 = x0*y0;
    uint64 t = x1*y0 + (w0>>32);
    uint64 w1 = (t<<32)>>32;
    uint64 w2 = t>>32;
    w1 += x0*y1;
    uint64 hi = x1*y1 + w2 + (w1>>32);
    return hi;
}

bool has_carry(uint64 A, uint64 B) {
    bool rtn = ((A&B)|((A|B)&(~(A+B))))>>63 == 1;
    return rtn;
}


void mul(bigInt* dst, bigInt A, bigInt B) {
    // Create matrix
    uint64 M[A.len][B.len][2];
    for (int i = 0; i < A.len; i++) {
        for (int j = 0; j < B.len; j++) {
            M[i][j][0] = mul_lo(A.data[i], B.data[j]);
            M[i][j][1] = mul_hi(A.data[i], B.data[j]);
        }
    }

    // Sum Matrix
    dst->len = A.len+B.len;
    dst->data = calloc(8, dst->len);
    uint64* carry = calloc(8, dst->len);
    for (int i = 0; i < A.len; i++) {
        for (int j = 0; j < B.len; j++) {
            for (int k = 0; k < 2; k++) {
                carry[i+j+k+1] += has_carry(dst->data[i+j+k], M[i][j][k]);
                dst->data[i+j+k] += M[i][j][k];
            }
        }
    }

    // Carry
    for (int i = 0; i < dst->len; i++) {
        carry[i+1] += has_carry(dst->data[i], carry[i]);
        dst->data[i] += carry[i];
    }

    // Trim zeros
    for (int i = dst->len-1; i >= 0; i--) {
        if (dst->data[i] != 0) {
            break;
        }
        dst->len -= 1;
    }
    dst->data = realloc(dst->data, (dst->len)<<3);
}

void show(uint64* num, int n) {
    for (int i = n-1; i >= 0; i--) {
        printf("%016lx ", num[i]);
    }
    printf("\n");
}

bool VERBOSE = false;

void test(bigInt nums[]) {
    bigInt A = nums[0];
    bigInt B = nums[1];
    bigInt dst_expected = nums[2];
    bigInt dst;

    /* BEGIN TIME */
    
    clock_t start = clock(), diff;

    mul(&dst, nums[0], nums[1]);

    diff = clock() - start;

    int msec = diff*1000/CLOCKS_PER_SEC;
    printf("%d\n", msec);


    /* END TIME */
    if (VERBOSE) {
        for (int i = 0; i < 3; i++) {
            show(nums[i].data, nums[i].len);
        }
        show(dst.data, dst.len);
        printf("\n");
    } else {
        printf("OK\n");
    }
    assert(dst.len == dst_expected.len);
    for (int i = 0; i < dst.len; i++) {
        assert(dst.data[i] == dst_expected.data[i]);
    }
    
    return;
}

int main() {
    int fp = open("../testdata/dynamic/testcases", O_RDONLY);
    int n;
    for (;;) {
        bigInt nums[3];
        for (int i = 0; i < 3; i++) {
            if (read(fp, &n, sizeof(n)) == sizeof(n)) {
                nums[i].data = calloc(8, (n+7)/8);
                if ((read(fp, nums[i].data, n) != n)) {
                    panic("Could not read Num");
                }
                nums[i].len = (n+7)/8;
            } else {
                goto DONE;
            }
        }
        test(nums);
    }
DONE:
    return 0;
}
