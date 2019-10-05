#include "b.h"
#include <stdio.h>

FuncAReturn func_a(FuncBReturn n) {
    printf("func_a(%d)\n", n);
    if (n > 0) {
        func_b(n - 1);
    }
    return 0;
}
