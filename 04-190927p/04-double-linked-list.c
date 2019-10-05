#include <stdio.h>
#include <stdlib.h>

typedef struct list_node {
    int data;
    struct list_node *prev;
    struct list_node *next;
} list_node;

typedef struct list {
    struct list_node *head;
    struct list_node *tail;
} list;

int main() {
    list_node *a = malloc(sizeof(list_node));
    list_node *b = malloc(sizeof(list_node));
    list_node *c = malloc(sizeof(list_node));

    a->next = b;
    b->next = c;
    c->next = NULL;
    a->prev = NULL;
    b->prev = a;
    c->prev = b;
    a->data = 10;
    b->data = 20;
    c->data = 30;

    list l;
    l.head = a;
    l.tail = c;
    printf("%d\n", l.head->data);
    printf("%d\n", l.head->next->data);
    printf("%d\n", l.head->next->next->data);
    for (list_node *cur = l.head; cur != NULL; cur = cur->next)
        printf("%p %d\n", cur, cur->data);
    for (list_node *cur = l.head; cur; cur = cur->next)
        printf("%p %d\n", cur, cur->data);

    free(c);
    free(b);
    free(a);
}

