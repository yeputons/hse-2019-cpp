#include "02_concat.h"
#include "02_concat.h"  // Test include guards.

#include "doctest.h"
#include <iostream>

//#define TEST_CONCAT_01_SINGLE
//#define TEST_CONCAT_02_MULTIPLE
//#define TEST_CONCAT_03_CUSTOM_OPERATOR
//#define TEST_CONCAT_04_NONMOVABLE

#ifdef TEST_CONCAT_01_SINGLE
TEST_CASE("concat formats a single value") {
    CHECK(cls_30::concat(1234) == "1234");
    CHECK(cls_30::concat(123.5) == "123.5");
    CHECK(cls_30::concat("hi") == "hi");
}
#endif  // TEST_CONCAT_01_SINGLE

#ifdef TEST_CONCAT_02_MULTIPLE
TEST_CASE("concat formats multiple values") {
    CHECK(cls_30::concat(12, 34) == "1234");
    CHECK(cls_30::concat(123.5, "hi", 12) == "123.5hi12");
}

TEST_CASE("concat formats zero values") {
    CHECK(cls_30::concat() == "");
}
#endif  // TEST_CONCAT_02_MULTIPLE

#ifdef TEST_CONCAT_03_CUSTOM_OPERATOR
struct SCustomOperator {
    int value;
    friend std::ostream& operator<<(std::ostream &os, const SCustomOperator &s) {
        return os << "|" << s.value << "|";
    }
};

TEST_CASE("concat formats multiple values with custom operator<<") {
    CHECK(cls_30::concat(12, SCustomOperator{10}, SCustomOperator{20}, 34) == "12|10||20|34");
}
#endif  // TEST_CONCAT_03_CUSTOM_OPERATOR

#ifdef TEST_CONCAT_04_NONMOVABLE
struct SNonmovable {
    int value;
    SNonmovable(int value_) : value(value_) {}
    SNonmovable(SNonmovable&&) = delete;
    friend std::ostream& operator<<(std::ostream &os, const SNonmovable &s) {
        return os << "|" << s.value << "|";
    }
};

TEST_CASE("concat formats multiple nonmovable values") {
    const SNonmovable a(10), b(20);
    CHECK(cls_30::concat(12, a, b, 34) == "12|10||20|34");
}
#endif  // TEST_CONCAT_04_NONMOVABLE
