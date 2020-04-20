#ifndef FOO_H_
#define FOO_H_

struct Foo {
    static inline const int N = 10;  // external linkage!
    static inline constexpr char Name[] = "NAME";  // external linkage до C++17
};

void foo();

#endif // FOO_H_
