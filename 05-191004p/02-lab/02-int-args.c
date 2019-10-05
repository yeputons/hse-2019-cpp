#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int sum = 0;
    for (int i = 0; i < argc; i++) {
        int x = atoi(argv[i]);
        printf("|%s| %d\n", argv[i], x);
        sum += x;
    }
    printf("%d\n", sum);
}
