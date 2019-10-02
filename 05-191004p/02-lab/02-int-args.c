#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    printf("argc=%d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("argv[%d] * 2 = %d\n", i, atoi(argv[i]) * 2);
    }
}
