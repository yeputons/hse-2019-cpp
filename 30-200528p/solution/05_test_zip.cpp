#include "05_zip.h"
#include "05_zip.h"  // Test include guards.

#include "doctest.h"
#include <boost/hana.hpp>
#include <utility>
#include <tuple>
#include <type_traits>

#define TEST_ZIP_01_EMPTY
#define TEST_ZIP_02_NON_EMPTY
#define TEST_ZIP_03_NON_ZIPPABLE_SFINAE

namespace {
auto is_zippable = boost::hana::is_valid(
    [](const auto &a, const auto &b)
        -> std::void_t<decltype(cls_30::zip(a, b))> {
    }
);
}

#ifdef TEST_ZIP_01_EMPTY
TEST_CASE("zip() works for empty tuples") {
    CHECK(cls_30::zip(std::tuple(), std::tuple()) == std::tuple());
}
#endif  // TEST_ZIP_01_EMPTY

#ifdef TEST_ZIP_02_NON_EMPTY
TEST_CASE("zip() works for non-empty tuples") {
    CHECK(
        cls_30::zip(
            std::tuple<int, char, char>(10, 'x', 'y'),
            std::tuple<double, float, int >(10.5, 10.75f, 11)
        ) == std::tuple(
            std::pair<int, double>(10, 10.5),
            std::pair<char, float>('x', 10.75f),
            std::pair<char, int>('y', 11)
        )
    );
}
#endif  // TEST_ZIP_02_NON_EMPTY

#ifdef TEST_ZIP_03_NON_ZIPPABLE_SFINAE
TEST_CASE("zip() gets SFINAEd out when tuples are of different sizes") {
    CHECK(is_zippable(std::tuple(10, 'x'), std::tuple(20, 'y')));
    CHECK(!is_zippable(std::tuple(), std::tuple(20, 'y')));
    CHECK(!is_zippable(std::tuple(10, 'x'), std::tuple(20, 'y', 'z')));
}
#endif  // TEST_ZIP_03_NON_ZIPPABLE_SFINAE
