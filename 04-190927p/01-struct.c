#include <stdio.h>

struct product {
    char name[256];
    int price_kop;
    char a;
};

int main() {
    int x;
    struct product p, q;
    int y;
    printf("sizeof(struct product)=%zu\n", sizeof(struct product));
    printf("&x=%p\n", &x);
    printf("&p=%p\n", &p);
    printf("&q=%p\n", &q);
    printf("&y=%p\n", &y);
}

