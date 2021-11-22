#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;
typedef byte* bytes;

void show(bytes bs, int n) {
    for (int i = 0; i < n; i++) {
        printf("%x", bs[i]);
    }
    printf("\n");
}

void panic(char* msg) {
    printf("%s\n", msg);
    exit(1);
}

void test(bytes* nums, int* nums_size) {
    printf("TEST\n");
    printf("A: ");
    show(nums[0], nums_size[0]);
    printf("B: ");
    show(nums[1], nums_size[1]);
    printf("C: ");
    show(nums[2], nums_size[2]);
    printf("\n\n");
    return;
}

int main() {
    int fp = open("testdata/small/numbers", O_RDONLY);
    int n;
    for (;;) {
        bytes nums[3];
        int nums_size[3];
        for (int i = 0; i < 3; i++) {
            if (read(fp, &n, sizeof(n)) == sizeof(n)) {
                nums[i] = malloc(n);
                if ((read(fp, nums[i], n) != n)) {
                    panic("Could not read Num");
                }
                nums_size[i] = n;
            } else {
                goto DONE;
            }
        }
        test(nums, nums_size);
    }
DONE:
    return 0;
}
