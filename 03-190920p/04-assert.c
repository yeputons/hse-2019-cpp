#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int add(int a, int b) {
    return a + b;
}

void test_add() {
//    abort();
//    exit(1);
    assert(add(2, 2) == 4);
    assert(add(2, 10) == 12);
}

int main() {
    test_add();
    printf("Tests pass\n");
    return 0;
}

