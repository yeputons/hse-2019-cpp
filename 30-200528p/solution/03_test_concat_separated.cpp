#include "03_concat_separated.h"
#include "03_concat_separated.h"  // Test include guards.

#include "doctest.h"
#include <functional>
#include <iostream>
#include <tuple>

#define TEST_CONCAT_SEPARATED_01_EMPTY_SEP
#define TEST_CONCAT_SEPARATED_02_NON_EMPTY_SEP
#define TEST_CONCAT_SEPARATED_03_CUSTOM_NONCOPYABLE
#define TEST_CONCAT_SEPARATED_04_TUPLE
#define TEST_CONCAT_SEPARATED_05_TUPLE_CUSTOM_NONCOPYABLE

#ifdef TEST_CONCAT_SEPARATED_01_EMPTY_SEP
TEST_CASE("concat_separated can work as a concat") {
    CHECK(cls_30::concat_separated("") == "");
    CHECK(cls_30::concat_separated("", 1234, 5678) == "12345678");
    CHECK(cls_30::concat_separated("", 1234, "hello", 5678) == "1234hello5678");
}
#endif  // TEST_CONCAT_SEPARATED_01_EMPTY_SEP

#ifdef TEST_CONCAT_SEPARATED_02_NON_EMPTY_SEP
TEST_CASE("concat_separated works") {
    CHECK(cls_30::concat_separated("<>") == "");
    CHECK(cls_30::concat_separated("<>", 1234, 5678) == "1234<>5678");
    CHECK(cls_30::concat_separated("<>", 1234, "hello", 5678) == "1234<>hello<>5678");
}
#endif  // TEST_CONCAT_SEPARATED_02_NON_EMPTY_SEP

#ifdef TEST_CONCAT_SEPARATED_03_CUSTOM_NONCOPYABLE
struct SNonmovable {
    int value;
    SNonmovable(int value_) : value(value_) {}
    SNonmovable(SNonmovable&&) = delete;
    friend std::ostream& operator<<(std::ostream &os, const SNonmovable &s) {
        return os << "|" << s.value << "|";
    }
};

TEST_CASE("concat_separated works with nonmovable values") {
    const SNonmovable a(10), b(20);
    CHECK(cls_30::concat_separated("<>", 12, a, b, 34) == "12<>|10|<>|20|<>34");
}
#endif  // TEST_CONCAT_SEPARATED_03_CUSTOM_NONCOPYABLE

#ifdef TEST_CONCAT_SEPARATED_04_TUPLE
TEST_CASE("concat_separated can take tuple<> as the second argument") {
    CHECK(cls_30::concat_separated("<>", std::tuple{}) == "");
    CHECK(cls_30::concat_separated("<>", std::tuple{1234, 5678}) == "1234<>5678");
    CHECK(cls_30::concat_separated("<>", std::tuple{1234, "hello", 5678}) == "1234<>hello<>5678");
}
#endif  // TEST_CONCAT_SEPARATED_04_TUPLE

#ifdef TEST_CONCAT_SEPARATED_05_TUPLE_CUSTOM_NONCOPYABLE
TEST_CASE("concat_separated can take tuple<> with references as the second argument") {
    const SNonmovable a(10), b(20);
    CHECK(cls_30::concat_separated("<>", std::make_tuple()) == "");
    CHECK(cls_30::concat_separated("<>", std::make_tuple(12, std::ref(a), std::ref(b), 34)) == "12<>|10|<>|20|<>34");
}
#endif  // TEST_CONCAT_SEPARATED_05_TUPLE_CUSTOM_NONCOPYABLE
