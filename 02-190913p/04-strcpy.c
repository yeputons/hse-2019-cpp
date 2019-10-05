#include <stdio.h>

void copy_string(char *src, char *dest) {
    while ((*dest++ = *src++));
/*
    int i = 0;
    while (src[i]) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = 0;
*/
}

int main() {
    char src[] = "Hello";
    char dest[10];
//    copy_string("abcdefg", dest);
    copy_string(src, dest);
    printf("%s\n", dest);
}
