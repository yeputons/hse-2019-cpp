#include <stdio.h>

void get_arr_ten(int start_value, int *out) {
    for (int i = 0; i < 10; i++)
        out[i] = start_value + i;
}

int main() {
    int arr0[10], arr100[10];
    get_arr_ten(0, arr0);
    get_arr_ten(100, arr100);
    printf("%p %p\n", arr0, arr100);
    for (int i = 0; i < 10; i++) {
        printf("%d\n", arr0[i]);
    }
    for (int i = 0; i < 10; i++) {
        printf("%d\n", arr100[i]);
    }
    return 0;
}

