#include <stdio.h>

int get_length(char *s) {
    int len = 0;
    while (*s++) len++;
    return len;
}

void print_str(char *s) {
    printf("%zu %d\n", sizeof s, get_length(s));
    for (int i = 0; i < 13; i++) {
        printf("s=%d\n", s[i]);
    }
    char c = 'c';
    printf("%d\n", get_length(&c)); // Undefined Behavior: out-of-range access inside get_length().
}

int main() {
    char s[] = "Hello World!";
    print_str(s);
    return 0;
}
