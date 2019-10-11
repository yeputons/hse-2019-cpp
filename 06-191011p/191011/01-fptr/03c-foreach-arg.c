#include <stdio.h>
#include <stddef.h>

void foreach(void *arr, size_t element_size, size_t elements, void (*action)(void*)) {
    for (size_t i = 0; i < elements; i++) {
        action(arr + element_size * i);
    }
}

void print_int(void *x) {
    printf("%p %d\n", x, *(int*)x);
}

void *add_sum_arg;
void add_sum(void *x) {
    int *sum_ptr = (int*)add_sum_arg;
    *sum_ptr += *(int*)x;
}

int main() {
    int arr[10];
    for (int i = 0; i < 10; i++)
        arr[i] = i * i;

    printf("arr=%p\n", arr);
    foreach(arr, sizeof arr[0], 10, print_int);
    /*for (int i = 0; i < 10; i++) {
        printf("%d\n", arr[i]);
    }*/

    /*int sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += arr[i];
    }*/
    int sum = 0;
    add_sum_arg = &sum;
    foreach(arr, sizeof arr[0], 10, add_sum);
    printf("%d\n", sum);
}

