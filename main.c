#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned long int dword;


void panic(char* msg) {
    printf("%s\n", msg);
    exit(1);
}

dword low64_mul(dword A, dword B) {
    return A*B;
}
dword hi64_mul(dword A, dword B) {
    dword mask32 = ((dword)1<<32)-1;
    dword out = 0;
    dword p0 = (A&mask32)*(B&mask32);
    dword p1 = (A>>32)*(B&mask32);
    dword p2 = (A&mask32)*(B&mask32);
    dword p3 = (A>>32)*(B>>32);
    dword cy = ((p0>>32) + (p1&mask32) + (p2&mask32))>>32;
    return p3+(p1>>32)+(p2>>32) + cy;
}

bool has_carry(dword A, dword B) {
    bool rtn = ((A&B)|((A|B)&(~(A+B))))>>63 == 1;
    return rtn;
}

void mul(dword* dst, dword* A, dword* B, int* dst_size, int size_A, int size_B) {
    dword M[size_A][size_B][2];
    for (int i = 0; i < size_A; i++) {
        for (int j = 0; j < size_B; j++) {
            for (int k = 0; k < 2; k++) {
                M[i][j][k] = 0;
            }
        }
    }
    for (int i = 0; i < size_A; i++) {
        for (int j = 0; j < size_B; j++) {
            M[i][j][0] = low64_mul(A[i], B[j]);
            M[i][j][1] = hi64_mul(A[i], B[j]);
        }
    }
    for (int i = 0; i < size_A+size_B; i++) {
        dst[i] = 0;
    }
    dword* carry = calloc(8, size_A+size_B);
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
    /*
    for (int i = 0; i < size_A+size_B; i++) {
        if (has_carry(dst[i], carry[i])) {
            carry[i+1] += 1;
        }
        dst[i] += carry[i];
    }
    */

    *dst_size = size_A+size_B;
    if (dst[size_A+size_B-1] == 0) {
        dst = realloc(dst, (size_A+size_B-1)<<3);
        *dst_size = size_A+size_B-1;
    }
}

void show(dword* num, int n) {
    for (int i = n-1; i >= 0; i--) {
        printf("%016lx ", num[i]);
    }
    printf("\n");
}

void test(dword* nums[], int* nums_size) {
    dword* D = calloc(8, nums_size[0]+nums_size[1]);
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
        dword* nums[3];
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
