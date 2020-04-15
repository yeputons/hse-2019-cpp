#include <cassert>
#include <cstdlib>
#include <iostream>

#pragma GCC diagnostic error "-Wwrite-strings"

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
    {
        Point3D *p3 = static_cast<Point3D*>(std::malloc(sizeof(Point3D)));
        Point2D *p2 = p3;
        p2->x = static_cast<int>((2 + 3) / 2);
        p2->y = 20;
        foo(static_cast<Point3D*>(p3));
        std::free(p3);
    }
    {
        Point3D *p3 = reinterpret_cast<Point3D*>(new char[sizeof(Point3D)]);
        Point2D *p2 = p3;
        p2->x = static_cast<int>((2 + 3) / 2);
        p2->y = 20;
        foo(static_cast<Point3D*>(p3));
        delete[] reinterpret_cast<char*>(p3);
    }
    {
        char s[] = "hi";
        legacy_foo(s);
    }
    {        
        legacy_foo(const_cast<char*>("hi"));
    }
    {        
        char *s = const_cast<char*>("hi");
        legacy_foo(s);
    }
    {
        const char *s = "hi";
        legacy_foo(const_cast<char*>(s));
    }
    std::cout << "PASSED\n";
}
