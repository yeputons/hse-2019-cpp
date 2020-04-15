#include <cassert>
#include <cstdlib>
#include <iostream>

#pragma GCC diagnostic error "-Wwrite-strings"
#pragma GCC diagnostic error "-Wold-style-cast"

struct Point2D { double x, y; };
struct Point3D : Point2D { double z; };

void foo(Point3D *p3) {
    p3->z = 10;
}

void legacy_foo(char *s) {
    assert(s[0] == 'h');
    assert(s[1] == 'i');
    assert(s[2] == 0);
}

int main() {
    // START SOLUTION
    {
        Point3D *p3 = (Point3D*)std::malloc(sizeof(Point3D));
        Point2D *p2 = (Point2D*)p3;
        p2->x = (int)((2 + 3) / 2);
        p2->y = (int)20;
        foo((Point3D*)p3);
        std::free(p3);
    }
    {
        Point3D *p3 = (Point3D*)new char[sizeof(Point3D)];
        Point2D *p2 = p3;
        p2->x = (int)((2 + 3) / 2);
        p2->y = (int)20;
        foo((Point3D*)p3);
        delete[] (char*)p3;
    }
    {
        char s[] = "hi";
        legacy_foo(s);
    }
    {        
        legacy_foo("hi");
    }
    {        
        char *s = "hi";
        legacy_foo(s);
    }
    {
        const char *s = "hi";
        legacy_foo(s);
    }
    // END SOLUTION
    std::cout << "PASSED\n";
}
