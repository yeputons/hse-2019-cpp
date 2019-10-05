#include <stdio.h>

void print_arr(int *a, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d\n", i);
        // TODO: a[i]
        // TODO: i[a]
    }
}

int main() {
    int arr[] = { 1, 2, 3, 4 };
    print_arr(arr, 4);
    return 0;
}

