#include <stdio.h>

void solve(const int n, const int *const arr) {
    printf("solving! %d\n", n);
    for (int i = 0; i < n; i++)
        printf("%d\n", arr[i]);
}

#define MAXN 100000 // C
// const int MAXN = 100000; // C++

int main() {
    int n;
    static int arr[MAXN + 1];
    while (scanf("%d", &n) == 1) {
        for (int i = 0; i < n; i++)
            scanf("%d", &arr[i]);
        solve(n, arr);
    }
    return 0;
}
