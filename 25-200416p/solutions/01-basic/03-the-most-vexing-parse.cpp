#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <utility>

struct Counter {
    Counter(int state_ = 0) : state(state_) {}

    int operator()() {
        return state++;
    }

private:
    int state = 0;
};

template<typename Gen>
struct Printer {
    Printer(Gen gen_) : gen(std::move(gen_)) {}

    friend auto& operator<<(std::ostream &os, Printer &p) {
        return os << p.gen();
    }

private:
    Gen gen;
};

int main() {
    {
        Printer<Counter> p(Counter(10));
        std::stringstream s;
        s << p << p << p;
        std::cout << "Test 1: '" << s.str() << "'" << std::endl;
        assert(s.str() == "101112");
    }
    {
        // START SOLUTION
        Printer<Counter> p{Counter{}};
        // END SOLUTION
        std::stringstream s;
        s << p << p << p;
        std::cout << "Test 2: '" << s.str() << "'" << std::endl;
        assert(s.str() == "012");
    }
    std::cout << "PASSED\n";
}
