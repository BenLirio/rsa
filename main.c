#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned long int uint64;


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

void mul(uint64* dst, uint64* A, uint64* B, int* dst_size, int size_A, int size_B) {
    uint64 M[size_A][size_B][2];
    for (int i = 0; i < size_A; i++) {
        for (int j = 0; j < size_B; j++) {
            for (int k = 0; k < 2; k++) {
                M[i][j][k] = 0;
            }
        }
    }
    for (int i = 0; i < size_A; i++) {
        for (int j = 0; j < size_B; j++) {
            M[i][j][0] = mul_lo(A[i], B[j]);
            M[i][j][1] = mul_hi(A[i], B[j]);
        }
    }
    for (int i = 0; i < size_A+size_B; i++) {
        dst[i] = 0;
    }
    uint64* carry = calloc(8, size_A+size_B);
    for (int i = 0; i < size_A; i++) {
        for (int j = 0; j < size_B; j++) {
            for (int k = 0; k < 2; k++) {
                if (has_carry(dst[i+j+k], M[i][j][k])) {
                    carry[i+j+k+1] += 1;
                }
                dst[i+j+k] += M[i][j][k];
            }
        }
    }
    for (int i = 0; i < size_A+size_B; i++) {
        if (has_carry(dst[i], carry[i])) {
            carry[i+1] += 1;
        }
        dst[i] += carry[i];
    }

    *dst_size = size_A+size_B;
    if (dst[size_A+size_B-1] == 0) {
        dst = realloc(dst, (size_A+size_B-1)<<3);
        *dst_size = size_A+size_B-1;
    }
}

void show(uint64* num, int n) {
    for (int i = n-1; i >= 0; i--) {
        printf("%016lx ", num[i]);
    }
    printf("\n");
}

void test(uint64* nums[], int* nums_size) {
    uint64* D = calloc(8, nums_size[0]+nums_size[1]);
    int D_size = 0;
    mul(D, nums[0], nums[1], &D_size, nums_size[0], nums_size[1]);
    for (int i = 0; i < 3; i++) {
        show(nums[i], nums_size[i]);
    }
    show(D, D_size);
    printf("\n");
    
    return;
}

int main() {
    int fp = open("testdata/small/numbers", O_RDONLY);
    int n;
    for (;;) {
        uint64* nums[3];
        int nums_size[3];
        for (int i = 0; i < 3; i++) {
            if (read(fp, &n, sizeof(n)) == sizeof(n)) {
                nums[i] = calloc(8, (n+7)/8);
                if ((read(fp, nums[i], n) != n)) {
                    panic("Could not read Num");
                }
                nums_size[i] = (n+7)/8;
            } else {
                goto DONE;
            }
        }
        test(nums, nums_size);
    }
DONE:
    return 0;
}
