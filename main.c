#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>

typedef unsigned char byte;
typedef unsigned short dByte;
typedef byte* bigInt;

void mul(bigInt C, bigInt A, bigInt B, int nbytes) {
    byte M[nbytes][nbytes][2];
    for (int i = 0; i < nbytes; i++) {
        for (int j = 0; j < nbytes; j++) {
            for (int k = 0; k < 2; k++) {
                M[i][j][k] = 0;
            }
        }
    }
    for (int i = 0; i < nbytes; i++) {
        for (int j = 0; j < nbytes; j++) {
            M[i][j][0] = A[i]*B[j];
            M[i][j][1] = ((byte)A[i]*B[j])>>(sizeof(byte)<<3);
        }
    }
    byte carry[nbytes<<1];
    for (int i = 0; i < nbytes<<1; i++) {
        carry[i] = 0;
    }
    for (int i = 0; i < nbytes; i++) {
        for (int j = 0; j < nbytes; j++) {
            for (int k = 0; k < 2; k++) {
                carry[i+j+k+1] += ((unsigned short)C[i+j+k] + M[i][j][k])>>8;
                C[i+j+k] += M[i][j][k];
            }
        }
    }
    for (int i = 0; i < nbytes<<1; i++) {
        if (i+1 < nbytes<<1) {
            carry[i+1] += ((unsigned short)C[i] + carry[i])>>4;
        }
        C[i] += carry[i];
    }
}

void show(bigInt A, int nbytes) {
    for (int i = nbytes-1; i >= 0; i--) {
        printf("%x", A[i]);
    }
    printf("\n");

}

int main() {
    bigInt A = malloc(128);
    bigInt B = malloc(128);
    bigInt C = malloc(256);

    int fp;

    fp = open("testdata/Dp", O_RDONLY);
    read(fp, A, 128);
    fp = open("testdata/Dq", O_RDONLY);
    read(fp, B, 128);

    mul(C, A, B, 128);
    show(A, 128);
    printf("\n");
    show(B, 128);
    printf("\n");
    show(C, 256);
    printf("\n");

    bigInt E = malloc(256);
    fp = open("testdata/N", O_RDONLY);
    read(fp, E, 256);
    show(E, 256);
    printf("\n");
}
