#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <cuda_runtime.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool VERBOSE = false;
bool MAP_HOST = true;

typedef unsigned long int uint64;

typedef struct bigInt {
    uint64* data;
    uint64* dev_data;
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

__global__ void kernal_create_table(uint64* M, bigInt* pA, bigInt* pB) {
    bigInt A = *pA;
    bigInt B = *pB;
    int i = threadIdx.x/B.len;
    int j = threadIdx.x%B.len;
    // cs.opensource.google/go/go/+/master:src/math/bits/bits.go
    // lo and hi multiplication for 64 bits
    uint64 x = A.data[i];
    uint64 y = B.data[j];
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
    *(M+(i*B.len*2)+(j*2)+(1)) = hi;
    *(M+(i*B.len*2)+(j*2)) = lo;
}

void mul(bigInt* pDst, bigInt* pA, bigInt* pB) {
    bigInt A = *pA;
    bigInt B = *pB;
    bigInt dst = *pDst;

    // Create matrix
    uint64* M;
    cudaHostAlloc(&M, A.len*B.len*2*sizeof(uint64), cudaHostAllocMapped);
    uint64 *dev_M;
    cudaHostGetDevicePointer(&dev_M, M, 0);
    bigInt* dev_pA;
    cudaHostGetDevicePointer(&dev_pA, pA, 0);
    cudaHostGetDevicePointer(&(*dev_pA).dev_data, A.data, 0);
    bigInt* dev_pB;
    cudaHostGetDevicePointer(&dev_pB, pB, 0);
    cudaHostGetDevicePointer(&(*dev_pB).dev_data, B.data, 0);

    kernal_create_table<<<1,A.len*B.len>>>(dev_M, dev_pA, dev_pB);
    cudaDeviceSynchronize();

    // Sum Matrix
    dst.len = A.len+B.len;
    dst.data = (uint64*)calloc(8, dst.len);
    ///cudaHostAlloc(&(dst.data), dst.len<<3, cudaHostAllocMapped);
    uint64* carry = (uint64*)calloc(8, dst.len);

    //bigInt* dev_pDst;
    //cudaHostGetDevicePointer(&dev_pDst, pDst, 0);
    //cudaHostGetDevicePointer(&(*dev_pDst).dev_data, dst.data, 0);
    for (int i = 0; i < A.len; i++) {
        for (int j = 0; j < B.len; j++) {
            for (int k = 0; k < 2; k++) {
                carry[i+j+k+1] += has_carry(dst.data[i+j+k], *(M+i*(B.len*2)+(j*2)+k));
                dst.data[i+j+k] += *(M+(i*B.len*2)+(j*2)+k);
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


void test(bigInt* pA, bigInt* pB, bigInt* pDst_expected) {
    bigInt A = *pA;
    bigInt B = *pB;
    bigInt dst_expected = *pDst_expected;
    bigInt* pDst;
    cudaHostAlloc(&pDst, sizeof(bigInt), cudaHostAllocMapped);
    mul(pDst, pA, pB);
    bigInt dst = *pDst;
    if (VERBOSE) {
        show(A.data, A.len);
        show(B.data, B.len);
        show(dst_expected.data, dst_expected.len);
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
    if (MAP_HOST) {
        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, 0);
        if (!prop.canMapHostMemory) {
            printf("Can not map host memory\n");
        }
        cudaSetDeviceFlags(cudaDeviceMapHost);
    }
    int fp = open("../testdata/small/numbers", O_RDONLY);
    int n;
    for (;;) {
        bigInt* nums[3];
        for (int i = 0; i < 3; i++) {
            cudaHostAlloc(&nums[i], sizeof(bigInt), cudaHostAllocMapped);
            if (read(fp, &n, sizeof(n)) == sizeof(n)) {
                cudaHostAlloc(&((*nums[i]).data), ((n+7)/8)*sizeof(uint64), cudaHostAllocMapped);
                if ((read(fp, (*nums[i]).data, n) != n)) {
                    assert(false);
                }
                (*nums[i]).len = (n+7)/8;
            } else {
                goto DONE;
            }
        }
        test(nums[0], nums[1], nums[2]);
    }
DONE:
    return 0;
}
