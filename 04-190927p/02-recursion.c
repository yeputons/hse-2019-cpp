#include <stdio.h>

struct big_struct {
    int a[1000];
};
typedef struct big_struct big_struct;

void eat_stack(int depth, big_struct s) {
    char c[2];
    (void)c;
    printf("%d %p\n", depth, &depth);
    eat_stack(depth + 1, s);
}

int main() {
    big_struct s;
    eat_stack(1, s);  // It will crash with depth=K. Ensure that stack size is about sizeof(big_struct)*K.
    // Next task: change stack size (`ulimit -r` command line command in Linux) and ensure that K changed.
}
