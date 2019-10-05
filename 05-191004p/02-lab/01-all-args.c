#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("addrs are %p %p\n", argc, argv);
    printf("argc=%d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("argv[%d]=%p |%s|\n", i, argv[i], argv[i]);
    }
}
