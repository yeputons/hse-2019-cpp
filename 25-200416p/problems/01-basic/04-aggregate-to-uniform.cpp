#include <cassert>
#include <iostream>

struct Point {
    int x, y;
    // START SOLUTION
    // END SOLUTION
};

int main() {
    {
        Point p;
        assert(p.x == 0);
        assert(p.y == 0);
    }
    {
        auto p = Point();
        assert(p.x == 0);
        assert(p.y == 0);
    }
    {
        Point p{};
        assert(p.x == 0);
        assert(p.y == 0);
    }
    {
        Point p{10, 20};
        assert(p.x == 10);
        assert(p.y == 20);
    }
    {
        #if 1
        // Should be compilation error.
        Point p{10};
        assert(p.x == 10);
        assert(p.y == 0);
        #endif
    }
    std::cout << "PASSED\n";
}
