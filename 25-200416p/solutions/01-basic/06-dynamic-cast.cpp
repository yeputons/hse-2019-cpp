#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>
#include <typeinfo>
#include <vector>

struct Figure {
    virtual ~Figure() {}
};

struct Circle : Figure {  // Non-aggregate!
    int radius;

    Circle(int radius_) : radius(radius_) {}
};

struct Rect : Figure {
    int width, height;

    Rect(int width_, int height_) : width(width_), height(height_) {}
};

int main() {
    std::vector<std::unique_ptr<Figure>> figures;
    figures.emplace_back(std::make_unique<Circle>(10));
    figures.emplace_back(std::make_unique<Rect>(3, 4));
    figures.emplace_back(std::make_unique<Circle>(20));
    figures.emplace_back(std::make_unique<Rect>(10, 15));
    figures.emplace_back(std::make_unique<Circle>(30));

    // dynamic_cast with pointers.
    assert(dynamic_cast<Circle*>(figures[0].get()));
    assert(!dynamic_cast<Rect*>(figures[0].get()));

    // dynamic_cast with references.
    static_cast<void>(dynamic_cast<Circle&>(*figures[0]));
    try {
        static_cast<void>(dynamic_cast<Rect&>(*figures[0]));
        assert(!"std::bad_cast expected");
    } catch (const std::bad_cast&) {
        // Do nothing, as expected.
    }

    // If statement with initializer: https://en.cppreference.com/w/cpp/language/if#If_Statements_with_Initializer
    if (auto circle0 = dynamic_cast<Circle*>(figures[0].get()); circle0) {
        assert(circle0->radius == 10);
    }

    std::stringstream out;

    // START SOLUTION
    for (const auto &fig : figures) {
        if (auto rect = dynamic_cast<Rect*>(fig.get()); rect) {
            out << "Rect(" << rect->width << ", " << rect->height << ")\n";
        } else {
            out << "Non-Rect\n";
        }
    }
    // END SOLUTION

    assert(out.str() ==
        "Non-Rect\n"
        "Rect(3, 4)\n"
        "Non-Rect\n"
        "Rect(10, 15)\n"
        "Non-Rect\n"
    );
    std::cout << "PASSED\n";
}
