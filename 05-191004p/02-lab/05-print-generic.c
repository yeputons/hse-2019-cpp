#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

void print_all(void *arr, int elements, size_t element_size, void (*print)(const void*)) {
    for (int i = 0; i < elements; i++) {
        if (i) printf(" ");
        print((char*)arr + element_size * i);
    }
    printf("\n");
}

void print_int(const void *x) {
    printf("%d", *(const int*)x);
}

int main(void) {
    int arr[] = { 1, 2, 3 };
    print_all(arr, sizeof arr / sizeof arr[0], sizeof arr[0], print_int);
}
