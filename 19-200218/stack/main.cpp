#include "assert.h"
#include <cstdio>
#include "my_stack.h"

using std::printf;

struct Watcher {
private:
    static int LastId, Alive;

    int id;
    int value = 0;

public:

    Watcher() : id(LastId++) {
        Alive++;
        printf("Watcher %d (val=%d) constructed by Watcher(), Alive=%d\n", id, value, Alive);
    }

    explicit Watcher(int _value) : id(LastId++), value(_value) {
        Alive++;
        printf("Watcher %d (val=%d) constructed by Watcher(int), Alive=%d\n", id, value, Alive);
    }

    Watcher(const Watcher &other) : id(LastId++), value(other.value) {
        Alive++;
        printf("Watcher %d (val=%d) constructed from %d (val=%d), Alive=%d\n", id, value, other.id, other.value, Alive);
    }

    Watcher& operator=(const Watcher &other) {
        printf("Watcher %d (val=%d) assigned from %d (val=%d)\n", id, value, other.id, other.value);
        value = other.value;
        return *this;
    }

    ~Watcher() {
        Alive--;
        printf("Watcher %d (val=%d) destructed, Alive=%d\n", id, value, Alive);
    }

    int getId() const { return id; }
    int getValue() const { return value; }
};

int Watcher::LastId = 1;
int Watcher::Alive = 0;

int main() {
    Watcher a, b(1000), c(2000), d(3000), e(4000);
    printf(
        "a=%d (val=%d), b=%d (val=%d), c=%d (val=%d), d=%d (val=%d), e=%d (val=%d)\n",
        a.getId(), a.getValue(),
        b.getId(), b.getValue(),
        c.getId(), c.getValue(),
        d.getId(), d.getValue(),
        e.getId(), e.getValue()
    );

    printf("Creating stk1\n");
    my_stack<Watcher> stk1;
    
    printf("Pushing a\n");
    stk1.push(a);
    
    printf("Pushing b\n");
    stk1.push(b);
    
    printf("Pushing c\n");
    stk1.push(c);

    printf("Copying stk1 to stk2 (constructor)\n");
    my_stack<Watcher> stk2 = stk1;

    printf("Pushing d to stk2\n");
    stk2.push(d);

    printf("Pushing e to stk2\n");
    stk2.push(e);
    
    for (int i = 0; i < 3; i++) {
        {
            printf("Getting top from stk1\n");
            Watcher t = stk1.top();
            printf("Top is %d (val=%d)\n", t.getId(), t.getValue());

            static const int expectedValues[] = { 2000, 1000, 0 };
            assert(t.getValue() == expectedValues[i]);
        }

        printf("Popping from stk1\n");
        stk1.pop();
    }

    printf("Copying stk2 to stk1 (assignment)\n");
    stk1 = stk2;
    
    for (int i = 0; i < 5; i++) {
        {
            printf("Getting top from stk1\n");
            Watcher t = stk1.top();
            printf("Top is %d (val=%d)\n", t.getId(), t.getValue());

            static const int expectedValues[] = { 4000, 3000, 2000, 1000, 0 };
            assert(t.getValue() == expectedValues[i]);
        }

        printf("Popping from stk1\n");
        stk1.pop();
    }
    printf("Destructing all stacks\n");
    return 0;
}
