#include <stdio.h>

int main() {
    int arr[10];
    for (int i = 0; i < 10; i++)
        arr[i] = i * i;

    for (int i = 0; i < 10; i++) {
        printf("%d\n", arr[i]);
    }

    int sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += arr[i];
    }
    printf("%d\n", sum);
}

