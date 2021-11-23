#include <fcntl.h>
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
    __asm__(
            "umulh x0, x0, x1"
   );
    register uint64 res asm("x0");
    return res;
}

bool has_carry(uint64 A, uint64 B) {
    bool rtn = ((A&B)|((A|B)&(~(A+B))))>>63 == 1;
    return rtn;
}


void mul(bigInt* dst, bigInt A, bigInt B) {
    dst->len = A.len+B.len;
    dst->data = calloc(8, dst->len);
    for (int i = 0; i < dst->len; i++) {
        dst->data[i] = 0;
    }
    uint64 M[A.len][B.len][2];
    for (int i = 0; i < A.len; i++) {
        for (int j = 0; j < B.len; j++) {
            for (int k = 0; k < 2; k++) {
                M[i][j][k] = 0;
            }
        }
    }
    for (int i = 0; i < A.len; i++) {
        for (int j = 0; j < B.len; j++) {
            M[i][j][0] = mul_lo(A.data[i], B.data[j]);
            M[i][j][1] = mul_hi(A.data[i], B.data[j]);
        }
    }
    uint64* carry = calloc(8, dst->len);
    for (int i = 0; i < A.len; i++) {
        for (int j = 0; j < B.len; j++) {
            for (int k = 0; k < 2; k++) {
                carry[i+j+k+1] += has_carry(dst->data[i+j+k], M[i][j][k]);
                dst->data[i+j+k] += M[i][j][k];
            }
        }
    }
    for (int i = 0; i < dst->len; i++) {
        carry[i+1] += has_carry(dst->data[i], carry[i]);
        dst->data[i] += carry[i];
    }
    if (dst->data[dst->len-1] == 0) {
        dst->len -= 1; 
        dst->data = realloc(dst->data, (dst->len)<<3);
    }
}

void show(uint64* num, int n) {
    for (int i = n-1; i >= 0; i--) {
        printf("%016lx ", num[i]);
    }
    printf("\n");
}

void test(bigInt nums[]) {
    bigInt* D = malloc(sizeof(bigInt));
    mul(D, nums[0], nums[1]);
    for (int i = 0; i < 3; i++) {
        show(nums[i].data, nums[i].len);
    }
    show(D->data, D->len);
    printf("\n");
    
    return;
}

int main() {
    int fp = open("testdata/small/numbers", O_RDONLY);
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
