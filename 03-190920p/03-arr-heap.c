#include <stdio.h>
#include <stdlib.h>

int *get_arr_ten(int start_value) {
    int *out = malloc(10 * sizeof(int));
    for (int i = 0; i < 10; i++)
        out[i] = start_value + i;
    return out;
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
    free(arr0);
    free(arr100);
    return 0;
}

