#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <cuda_runtime.h>
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

void mul_hi(uint64* dst, uint64 A, uint64 B) {
    __asm__(
            "umulh x1, x1, x2\n"
            "str x1, [x0]"
   );
}

bool has_carry(uint64 A, uint64 B) {
    bool rtn = ((A&B)|((A|B)&(~(A+B))))>>63 == 1;
    return rtn;
}

__global__ void kernal_create_table(uint64* M, uint64* A_data, uint64* B_data, int A_len, int B_len) {
    int i = threadIdx.x/B_len;
    int j = threadIdx.x%B_len;
    // cs.opensource.google/go/go/+/master:src/math/bits/bits.go
    // lo and hi multiplication for 64 bits
    uint64 x = A_data[i];
    uint64 y = B_data[j];
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
    uint64 lo = x*y;
    *(M+(i*B_len*2)+(j*2)+(1)) = hi;
    *(M+i*B_len*2+j*2) = lo;
}


void mul(bigInt* pDst, bigInt A, bigInt B) {
    bigInt dst = *pDst;
    // Create matrix
    uint64 M[A.len][B.len][2];
    uint64 *dev_M;
    size_t bytes = A.len*B.len*2*sizeof(uint64);
    cudaMalloc(&dev_M, bytes);
    cudaMemcpy(dev_M, M, bytes, cudaMemcpyHostToDevice);
    uint64 *dev_A_data;
    cudaMalloc(&dev_A_data, A.len*sizeof(uint64));
    cudaMemcpy(dev_A_data, A.data, A.len*sizeof(uint64), cudaMemcpyHostToDevice);
    uint64 *dev_B_data;
    cudaMalloc(&dev_B_data, B.len*sizeof(uint64));
    cudaMemcpy(dev_B_data, B.data, B.len*sizeof(uint64), cudaMemcpyHostToDevice);
    kernal_create_table<<<1,A.len*B.len>>>(dev_M, dev_A_data, dev_B_data, A.len, B.len);

    cudaMemcpy(M, dev_M, bytes, cudaMemcpyDeviceToHost);


    // Sum Matrix
    dst.len = A.len+B.len;
    dst.data = (uint64*)calloc(8, dst.len);
    uint64* carry = (uint64*)calloc(8, dst.len);
    for (int i = 0; i < A.len; i++) {
        for (int j = 0; j < B.len; j++) {
            for (int k = 0; k < 2; k++) {
                carry[i+j+k+1] += has_carry(dst.data[i+j+k], M[i][j][k]);
                dst.data[i+j+k] += M[i][j][k];
            }
        }
    }

    // Carry
    for (int i = 0; i < dst.len; i++) {
        carry[i+1] += has_carry(dst.data[i], carry[i]);
        dst.data[i] += carry[i];
    }

    // Trim zeros
    for (int i = dst.len-1; i >= 0; i--) {
        if (dst.data[i] != 0) {
            break;
        }
        dst.len -= 1;
    }
    dst.data = (uint64*)realloc(dst.data, (dst.len)<<3);

    *pDst = dst;
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
    mul(&dst, A, B);
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
    int fp = open("testdata/small/numbers", O_RDONLY);
    int n;
    for (;;) {
        bigInt nums[3];
        for (int i = 0; i < 3; i++) {
            if (read(fp, &n, sizeof(n)) == sizeof(n)) {
                nums[i].data = (uint64*)calloc(8, (n+7)/8);
                if ((read(fp, nums[i].data, n) != n)) {
                    assert(false);
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
