#include <stdio.h>

void print_int(int x) {
    printf("%d\n", x);
}

// 1.
/* ? */ print_int_arr(/* ... */) {
   // ???
}

// 2. print_int(int) --> print_int(int*) + const?
// 2b. print_int_arr(int*) --> print_int_arr(void*)
// 3. print_int_arr --> print_arr(void*, ..., void (*print)(void*));

int main() {
    int arr[] = { 2, 1, 3 };
    for (int i = 0; i < 3; i++) {
        print_int(arr[i]);
    }
}
