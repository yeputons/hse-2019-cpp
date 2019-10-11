#include <stdio.h>
#include <stddef.h>

void foreach(void *arr, size_t element_size, size_t elements, void (*action)(void*, void*), void* action_data) {
    for (size_t i = 0; i < elements; i++) {
        action(action_data, arr + element_size * i);
    }
}

void print_int(void *unused, void *x) {
    (void)unused;
    printf("%p %d\n", x, *(int*)x);
}

void add_sum(void *sum, void *x) {
    *(int*)sum += *(int*)x;
}

// TODO: mul_prod
// TODO: add_sum_float

int main() {
    int arr[10];
    for (int i = 0; i < 10; i++)
        arr[i] = i * i;

    printf("arr=%p\n", arr);
    foreach(arr, sizeof arr[0], 10, print_int, NULL);
    /*for (int i = 0; i < 10; i++) {
        printf("%d\n", arr[i]);
    }*/

    /*int sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += arr[i];
    }*/
    int sum = 0;
    foreach(arr, sizeof arr[0], 10, add_sum, &sum);
    printf("%d\n", sum);
}

