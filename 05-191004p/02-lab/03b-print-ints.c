#include <stdio.h>
#include <stdlib.h>

void print_int(int x) {
    printf("%d", x);
}

int main() {
    int arr[] = { 1, 2, 3 };
    for (int i = 0; i < 3; i++) {
        if (i) printf(" ");
        print_int(arr[i]);
    }
    printf("\n");
}
