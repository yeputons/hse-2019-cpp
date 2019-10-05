#include <stdio.h>

int arr[10]; // Global array.

int *get_arr_ten(int start_value) {
    // int arr[10];  // Accessing local array after function returns is UB.
    for (int i = 0; i < 10; i++)
        arr[i] = start_value + i;
    return arr;
}

int main() {
    int *arr0 = get_arr_ten(0);
    int *arr100 = get_arr_ten(100);
    printf("%p %p\n", arr0, arr100);
    for (int i = 0; i < 10; i++) {
        printf("%d\n", arr0[i]);
    }
    for (int i = 0; i < 10; i++) {
        printf("%d\n", arr100[i]);
    }
    return 0;
}

