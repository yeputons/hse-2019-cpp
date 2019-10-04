#include <stdio.h>

int main() {
    // const char s[] = "hello";  // Это уже не указатель, арифметика не работает.
    char s_data[] = "hello";
    char *const s = s_data;
    printf("%p %c %s\n", s, *s, s);
    *s = 'H';
    s[0] = 'H';

    s++;
    printf("%p %c %s\n", s, *s, s);
    *s = 'H';
    s[0] = 'H';
}
