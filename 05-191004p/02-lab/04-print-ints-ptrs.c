#include <stdio.h>
#include <stdlib.h>

void print_int(const int *const x) {
    printf("%d", *x);
}

void print_all(const int *const arr, int elements) {
    for (int i = 0; i < elements; i++) {
        if (i) printf(" ");
        print_int(&arr[i]);
    }
    printf("\n");
}

int main(void) {
    int arr[] = { 1, 2, 3 };
    print_all(arr, sizeof arr / sizeof arr[0]);
}
