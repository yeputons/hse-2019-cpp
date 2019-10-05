#include "a.h"
#include <stdio.h>

int func_b(int n) {
    printf("func_b(%d)\n", n);
    if (n > 0) {
        func_a(n - 1);
    }
    return 0;
}
