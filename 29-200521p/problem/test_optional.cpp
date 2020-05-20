#include "optional.h"
#include "optional.h"  // Test double inclusion

#include "doctest.h"
#include <memory>
#include <string>
#include <utility>

#define OPTIONAL_TEST_01_INIT
#define OPTIONAL_TEST_02_OWNERSHIP
#define OPTIONAL_TEST_03_RESET
#define OPTIONAL_TEST_04_MOVABLE
#define OPTIONAL_TEST_05_COPYABLE
//#define OPTIONAL_TEST_06_EMPLACE_1_BY_VALUE
//#define OPTIONAL_TEST_07_EMPLACE_1
//#define OPTIONAL_TEST_08_EMPLACE

#ifdef OPTIONAL_TEST_01_INIT
TEST_CASE("Default-initialize optional") {
    cls_29::optional<std::string> x;
    CHECK(!x.has_value());
}

TEST_CASE("Copy-initialize from value") {
    std::string s(10000, 'x');
    cls_29::optional<std::string> x(s);
    REQUIRE(x.has_value());
    CHECK((x.value() == std::string(10000, 'x')));
    CHECK((s == std::string(10000, 'x')));
}

TEST_CASE("Move-initialize from value") {
    cls_29::optional<std::string> x(std::string(10000, 'x'));
    REQUIRE(x.has_value());
    CHECK((x.value() == std::string(10000, 'x')));
}
#endif  // OPTIONAL_TEST_01_INIT

#ifdef OPTIONAL_TEST_02_OWNERSHIP
TEST_CASE("Optional owns the object") {
    int destructors = 0;
    struct DestructCounter {
        int *destructorsCounter;

        ~DestructCounter() { ++*destructorsCounter; }
    };

    {
        DestructCounter counter{&destructors};
        REQUIRE(destructors == 0);
        {
            cls_29::optional<DestructCounter> x(counter);
            static_cast<void>(x);
            CHECK(destructors == 0);
        }
        CHECK(destructors == 1);
    }
    CHECK(destructors == 2);
}
#endif  // OPTIONAL_TEST_02_OWNERSHIP

#ifdef OPTIONAL_TEST_03_RESET
TEST_CASE("reset() works for empty optional") {
    cls_29::optional<std::string> x;
    x.reset();
    CHECK(!x.has_value());
}

TEST_CASE("reset() works for non-empty optional") {
    cls_29::optional<std::string> x(std::string(10000, 'x'));
    x.reset();
    CHECK(!x.has_value());
}
#endif  // OPTIONAL_TEST_03_RESET

#ifdef OPTIONAL_TEST_04_MOVABLE
TEST_CASE("optional is movable") {
    cls_29::optional<std::string> empty;
    cls_29::optional<std::string> withData(std::string(10000, 'x'));

    SUBCASE("move-constructible from empty") {
        cls_29::optional x(std::move(empty));
        CHECK(!empty.has_value());
        CHECK(!x.has_value());
    }

    SUBCASE("move-assignable from empty to empty") {
        cls_29::optional<std::string> x;
        CHECK(&(x = std::move(empty)) == &x);
        CHECK(!empty.has_value());
        CHECK(!x.has_value());
    }

    SUBCASE("move-assignable from empty to data") {
        cls_29::optional x(std::string(10000, 'y'));
        CHECK(&(x = std::move(empty)) == &x);
        CHECK(!empty.has_value());
        CHECK(!x.has_value());
    }

    SUBCASE("move-constructible from data") {
        cls_29::optional x(std::move(withData));
        CHECK(withData.has_value());
        REQUIRE(x.has_value());
        CHECK((x.value() == std::string(10000, 'x')));
    }

    SUBCASE("move-assignable from data to empty") {
        cls_29::optional<std::string> x;
        CHECK(&(x = std::move(withData)) == &x);
        CHECK(withData.has_value());
        REQUIRE(x.has_value());
        CHECK((x.value() == std::string(10000, 'x')));
    }

    SUBCASE("move-assignable from data to data") {
        cls_29::optional x(std::string(10000, 'y'));
        CHECK(&(x = std::move(withData)) == &x);
        CHECK(withData.has_value());
        REQUIRE(x.has_value());
        CHECK((x.value() == std::string(10000, 'x')));
    }
}
#endif  // OPTIONAL_TEST_04_MOVABLE

#ifdef OPTIONAL_TEST_05_COPYABLE
TEST_CASE("optional is copyable") {
    cls_29::optional<std::string> empty;
    cls_29::optional<std::string> withData(std::string(10000, 'x'));

    SUBCASE("copy-constructible from empty") {
        cls_29::optional x(empty);
        CHECK(!empty.has_value());
        CHECK(!x.has_value());
    }

    SUBCASE("copy-assignable from empty to empty") {
        cls_29::optional<std::string> x;
        CHECK(&(x = empty) == &x);
        CHECK(!empty.has_value());
        CHECK(!x.has_value());
    }

    SUBCASE("copy-assignable from empty to data") {
        cls_29::optional x(std::string(10000, 'y'));
        CHECK(&(x = empty) == &x);
        CHECK(!empty.has_value());
        CHECK(!x.has_value());
    }

    SUBCASE("copy-constructible from data") {
        cls_29::optional x(withData);
        REQUIRE(withData.has_value());
        CHECK((withData.value() == std::string(10000, 'x')));
        REQUIRE(x.has_value());
        CHECK((x.value() == std::string(10000, 'x')));
    }

    SUBCASE("copy-assignable from data to empty") {
        cls_29::optional<std::string> x;
        CHECK(&(x = withData) == &x);
        REQUIRE(withData.has_value());
        CHECK((withData.value() == std::string(10000, 'x')));
        REQUIRE(x.has_value());
        CHECK((x.value() == std::string(10000, 'x')));
    }

    SUBCASE("copy-assignable from data to data") {
        cls_29::optional x(std::string(10000, 'y'));
        CHECK(&(x = withData) == &x);
        REQUIRE(withData.has_value());
        CHECK((withData.value() == std::string(10000, 'x')));
        REQUIRE(x.has_value());
        CHECK((x.value() == std::string(10000, 'x')));
    }
}
#endif  // OPTIONAL_TEST_05_COPYABLE

#ifdef OPTIONAL_TEST_06_EMPLACE_1_BY_VALUE
TEST_CASE("emplace() works with std::string") {
    cls_29::optional<std::string> x;
    x.emplace("hello world");
    REQUIRE(x.has_value());
    CHECK(x.value() == "hello world");
}

TEST_CASE("emplace() works with 1 argument by-value") {
    struct S {
        bool initialized = false;
        S(const char*) : initialized(true) {}
    };
    cls_29::optional<S> x;
    x.emplace("hello world");
    REQUIRE(x.has_value());
    CHECK(x.value().initialized);
}

TEST_CASE("emplace() works with 1 move-only argument") {
    cls_29::optional<std::unique_ptr<int>> x;
    x.emplace(std::unique_ptr<int>());
    REQUIRE(x.has_value());
    CHECK(!x.value());
}
#endif  // OPTIONAL_TEST_06_EMPLACE_1_BY_VALUE

#ifdef OPTIONAL_TEST_07_EMPLACE_1
TEST_CASE("emplace() correctly preserves category of 1 argument") {
    struct S {
        int constructedBy = 0;
        S(int&) : constructedBy(1) {}
        S(int&&) : constructedBy(2) {}
        S(S&&) = delete;
    };

    cls_29::optional<S> x;
    int a = 10;

    SUBCASE("int&") {
        x.emplace(a);
        REQUIRE(x.has_value());
        CHECK(x.value().constructedBy == 1);
    }

    SUBCASE("int&&") {
        x.emplace(std::move(a));
        REQUIRE(x.has_value());
        CHECK(x.value().constructedBy == 2);
    }
}
#endif  // OPTIONAL_TEST_07_EMPLACE_1

#ifdef OPTIONAL_TEST_08_EMPLACE
TEST_CASE("emplace() works with 1 argument") {
    cls_29::optional<std::string> x;
    x.emplace("hello world");
    REQUIRE(x.has_value());
    CHECK(x.value() == "hello world");
}

TEST_CASE("emplace() works with 2 argument") {
    cls_29::optional<std::string> x;
    x.emplace(10, 'x');
    REQUIRE(x.has_value());
    CHECK((x.value() == std::string(10, 'x')));
}

TEST_CASE("emplace() correctly preserves category of 2 arguments") {
    struct S {
        int constructedBy = 0;
        S(int&, int&) : constructedBy(1) {}
        S(int&, int&&) : constructedBy(2) {}
        S(int&&, int&) : constructedBy(3) {}
        S(int&&, int&&) : constructedBy(4) {}
        S(S&&) = delete;
    };

    cls_29::optional<S> x;
    int a = 10, b = 20;

    SUBCASE("int&, int&") {
        x.emplace(a, b);
        REQUIRE(x.has_value());
        CHECK(x.value().constructedBy == 1);
    }

    SUBCASE("int&, int&&") {
        x.emplace(a, std::move(b));
        REQUIRE(x.has_value());
        CHECK(x.value().constructedBy == 2);
    }

    SUBCASE("int&&, int&") {
        x.emplace(std::move(a), b);
        REQUIRE(x.has_value());
        CHECK(x.value().constructedBy == 3);
    }

    SUBCASE("int&&, int&&") {
        x.emplace(std::move(a), std::move(b));
        REQUIRE(x.has_value());
        CHECK(x.value().constructedBy == 4);
    }
}
#endif  // OPTIONAL_TEST_08_EMPLACE
