#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

enum PRECOMPUTED {M_IDX, E_IDX, N_IDX, D_IDX, C_IDX};

typedef unsigned long int uint64;
typedef uint64* bigInt;

void printBigInt(bigInt val, int size) {
    for (int i = size-1; i >= 0; i--) {
        printf("%.16lx ", val[i]);
        if (i != size-1 && i%4 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void get_precomputed(bigInt* dst, enum PRECOMPUTED idx) {
    char file_name[128];
    switch(idx) {
        case M_IDX:
            strcpy(file_name, "../testdata/rsa/M");
            break;
        case E_IDX:
            strcpy(file_name, "../testdata/rsa/E");
            break;
        case N_IDX:
            strcpy(file_name, "../testdata/rsa/N");
            break;
        case D_IDX:
            strcpy(file_name, "../testdata/rsa/D");
            break;
        default:
            printf("No valid precomputed value with idx %d\n", idx);
            exit(1);
            break;

    }
    int fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Error: Could not open file. %s.\n", strerror(errno));
    }
    int n;
    n = read(fd, *dst, (2048>>3));
    if (n != (2048>>3)) {
        fprintf(stderr, "Error: Could not read file. %s.\n", strerror(errno));
    }
}


void Exp(bigInt dst, bigInt b, bigInt e, bigInt m) {

}


int main() {
    bigInt M = malloc(2048>>3);
    get_precomputed(&M, M_IDX);

    bigInt E = malloc(2048>>3);
    get_precomputed(&E, E_IDX);

    bigInt N = malloc(2048>>3);
    get_precomputed(&N, N_IDX);

    bigInt D = malloc(2048>>3);
    get_precomputed(&D, D_IDX);

    bigInt C = calloc(1, 2048>>3);
    Exp(C, M, E, N);
    bigInt Mp = calloc(1,2048>>3);
    Exp(Mp, C, D, N);
    printf("M:\n");
    printBigInt(M, 2048>>6);
    printf("C:\n");
    printBigInt(C, 2048>>6);
    printf("Mp:\n");
    printBigInt(Mp, 2048>>6);
}
