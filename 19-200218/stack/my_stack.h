#ifndef MY_STACK_H_
#define MY_STACK_H_

#include <cstddef>

template<typename T>
struct my_stack {
private:
    T *data;
    std::size_t len, cap;

public:
    my_stack() noexcept;
    my_stack(const my_stack&);
    my_stack& operator=(const my_stack&);
    ~my_stack();

    bool empty() const noexcept;
    std::size_t length() const noexcept;

    void reserve(std::size_t newcap);

    void push(const T&);
    void pop();
    T top() const;
};

#include "my_stack_impl.h"

#endif
