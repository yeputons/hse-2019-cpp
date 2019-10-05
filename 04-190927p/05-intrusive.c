#include <stdio.h>
#include <stddef.h>

typedef struct list_node {
   struct list_node *next;
} list_node;

struct data {
    int value;
    struct list_node entry;
};

int main() {
    struct data a, b, c;
    a.entry.next = &b.entry;
    b.entry.next = &c.entry;
    c.entry.next = NULL;
    a.value = 10;
    b.value = 20;
    c.value = 30;

    list_node *second_node = a.entry.next;
    int offset = offsetof(struct data, entry);
    printf("%d\n", offset);

    struct data *d = (struct data*)((char*)second_node - offset);
    printf("%d\n", d->value);
    printf("%p %p\n", second_node, &d->entry);
    return 0;
}

