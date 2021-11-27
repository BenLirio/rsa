#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "./types.h"
#include "./exp.h"

enum PRECOMPUTED {M_IDX, E_IDX, N_IDX, D_IDX, C_IDX, M_2_IDX, M_3_IDX, M_4_IDX, M_5_IDX, M_6_IDX, M_7_IDX, M_8_IDX, M_9_IDX, M_10_IDX, M_11_IDX, M_12_IDX};

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
        case C_IDX:
            strcpy(file_name, "../testdata/rsa/C");
            break;
        case M_2_IDX:
            strcpy(file_name, "../testdata/rsa/M_2");
            break;
        case M_3_IDX:
            strcpy(file_name, "../testdata/rsa/M_3");
            break;
        case M_4_IDX:
            strcpy(file_name, "../testdata/rsa/M_4");
            break;
        case M_5_IDX:
            strcpy(file_name, "../testdata/rsa/M_5");
            break;
        case M_6_IDX:
            strcpy(file_name, "../testdata/rsa/M_6");
            break;
        case M_7_IDX:
            strcpy(file_name, "../testdata/rsa/M_7");
            break;
        case M_8_IDX:
            strcpy(file_name, "../testdata/rsa/M_8");
            break;
        case M_9_IDX:
            strcpy(file_name, "../testdata/rsa/M_9");
            break;
        case M_10_IDX:
            strcpy(file_name, "../testdata/rsa/M_10");
            break;
        case M_11_IDX:
            strcpy(file_name, "../testdata/rsa/M_11");
            break;
        case M_12_IDX:
            strcpy(file_name, "../testdata/rsa/M_12");
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

int main() {
    bigInt M = malloc(2048>>3);
    get_precomputed(&M, M_IDX);

    bigInt E = malloc(2048>>3);
    get_precomputed(&E, E_IDX);

    bigInt N = malloc(2048>>3);
    get_precomputed(&N, N_IDX);

    bigInt D = malloc(2048>>3);
    get_precomputed(&D, D_IDX);

    bigInt C_expected = malloc(2048>>3);
    get_precomputed(&C_expected, C_IDX);

    bigInt C = calloc(1, 2048>>3);
    Exp(C, M, E, N);
    bigInt Mp = calloc(1,2048>>3);
    Exp(Mp, C, D, N);
    printf("M:\n");
    printBigInt(M, 2048>>6);
    printf("C:\n");
    printBigInt(C, 2048>>6);
    printf("C_expected:\n");
    printBigInt(C_expected, 2048>>6);
    printf("Mp:\n");
    printBigInt(Mp, 2048>>6);

    free(M);
    free(N);
    free(D);
    free(E);
}
