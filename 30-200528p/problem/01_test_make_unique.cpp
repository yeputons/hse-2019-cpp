#include "01_make_unique.h"
#include "01_make_unique.h"  // Test include guards.

#include "doctest.h"
#include <memory>
#include <utility>
#include <vector>

//#define TEST_MAKE_UNIQUE_01_SINGLE_BY_VALUE
//#define TEST_MAKE_UNIQUE_02_SINGLE_PRESERVE_CATEGORY
//#define TEST_MAKE_UNIQUE_03_MULTIPLE_BY_VALUE
//#define TEST_MAKE_UNIQUE_04_MULTIPLE_PRESERVE_CATEGORY

struct nonmovable {
    nonmovable() = default;
    nonmovable(nonmovable&&) = delete;
};

#ifdef TEST_MAKE_UNIQUE_01_SINGLE_BY_VALUE
TEST_CASE("make_unique takes single argument by-value") {
    struct S : nonmovable {
        int value;
        S(int value_) : value(value_) {}
    };

    std::unique_ptr<S> ptr = cls_30::make_unique<S>(10);
    REQUIRE(ptr);
    CHECK(ptr->value == 10);
}
#endif  // TEST_MAKE_UNIQUE_01_SINGLE_BY_VALUE

#ifdef TEST_MAKE_UNIQUE_02_SINGLE_PRESERVE_CATEGORY
TEST_SUITE("make_unique takes single argument and preserves category") {
TEST_CASE("by reference") {
    struct S : nonmovable {
        int &value;
        S(int &value_) : value(value_) {}
    };

    int x = 10;
    std::unique_ptr<S> ptr = cls_30::make_unique<S>(x);
    REQUIRE(ptr);
    CHECK(&ptr->value == &x);
}

TEST_CASE("by const reference") {
    struct S : nonmovable {
        const int &value;
        S(const int &value_) : value(value_) {}
    };

    const int x = 10;
    std::unique_ptr<S> ptr = cls_30::make_unique<S>(x);
    REQUIRE(ptr);
    CHECK(&ptr->value == &x);
}

TEST_CASE("by rvalue reference") {
    struct S : nonmovable {
        int &&value;
        S(int &&value_) : value(std::move(value_)) {}
    };

    int x = 10;
    std::unique_ptr<S> ptr = cls_30::make_unique<S>(std::move(x));
    REQUIRE(ptr);
    CHECK(&ptr->value == &x);
}
}
#endif  // TEST_MAKE_UNIQUE_02_SINGLE_PRESERVE_CATEGORY

#ifdef TEST_MAKE_UNIQUE_03_MULTIPLE_BY_VALUE
TEST_CASE("make_unique takes multiple arguments by-value") {
    struct S : nonmovable {
        std::vector<int> values;
        S() : values{} {};
        S(int a) : values{a} {};
        S(int a, int b) : values{a, b} {};
        S(int a, int b, int c) : values{a, b, c} {};
    };

    SUBCASE("zero arguments") {
        std::unique_ptr<S> ptr = cls_30::make_unique<S>();
        REQUIRE(ptr);
        CHECK((ptr->values == std::vector<int>{}));
    }

    SUBCASE("1 argument") {
        std::unique_ptr<S> ptr = cls_30::make_unique<S>(10);
        REQUIRE(ptr);
        CHECK((ptr->values == std::vector<int>{10}));
    }

    SUBCASE("2 arguments") {
        std::unique_ptr<S> ptr = cls_30::make_unique<S>(10, 20);
        REQUIRE(ptr);
        CHECK((ptr->values == std::vector<int>{10, 20}));
    }

    SUBCASE("3 arguments") {
        std::unique_ptr<S> ptr = cls_30::make_unique<S>(10, 20, 30);
        REQUIRE(ptr);
        CHECK((ptr->values == std::vector<int>{10, 20, 30}));
    }
}
#endif  // TEST_MAKE_UNIQUE_03_MULTIPLE_BY_VALUE

#ifdef TEST_MAKE_UNIQUE_04_MULTIPLE_PRESERVE_CATEGORY
TEST_CASE("make_unique takes multiple arguments by-value") {
    struct S : nonmovable {
        int a;
        int &b;
        int &&c;
        const int &d;
        S(int a, int &b_, int &&c_, const int &d_) : a(a), b(b_), c(std::move(c_)), d(d_) {}
    };

    int x = 20;
    int y = 30;
    const int z = 10;
    std::unique_ptr<S> ptr = cls_30::make_unique<S>(10, x, std::move(y), z);
    REQUIRE(ptr);
    CHECK(ptr->a == 10);
    CHECK(&ptr->b == &x);
    CHECK(&ptr->c == &y);
    CHECK(&ptr->d == &z);
}
#endif  // TEST_MAKE_UNIQUE_04_MULTIPLE_PRESERVE_CATEGORY
