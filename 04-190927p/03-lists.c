#include <stdio.h>

typedef struct list_node {
    int data;
    struct list_node *next;
} list_node;

int main() {
    list_node a, b, c;
    a.data = 10;
    a.next = &b;

    b.data = 20;
    b.next = &c;

    c.data = 30;
    c.next = NULL;

    printf("%d %p\n", a.data, &a.data);

    printf("%d %p\n", (*a.next).data, &(*a.next).data);
    printf("%d %p\n", a.next->data, &a.next->data);
    printf("%d %p\n", b.data, &b.data);

    printf("%d %p\n", a.next->next->data, &a.next->next->data);
}
