#include <stdio.h>

int main() {
    char *const s = "hello";
    printf("%p %c %s\n", s, *s, s);
    *s = 'H';
    s[0] = 'H';

//    s++;
    printf("%p %c %s\n", s, *s, s);
    *s = 'H';
    s[0] = 'H';
}
