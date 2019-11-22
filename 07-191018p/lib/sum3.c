#include "sum3.h"
#include "sum.h"

int sum3(int a, int b, int c) {
    return sum(sum(a, b), c);
}
