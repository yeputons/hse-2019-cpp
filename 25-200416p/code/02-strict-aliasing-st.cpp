struct s { int a; };
struct t { int b; };
int st1(struct s *p, struct t *q) {
    p->a = 1;
    q->b = 2;
    return p->a;  // GCC optimizes to 'return 1'.
}

int st2(struct s *p, struct t *q) {
    int *pa = & (p->a);
    int *qb = & (q->b);
    *pa = 1;
    *qb = 2;
    return *pa;  // GCC does not optimize to 'return 1'.
}
