#include "any.h"
#include "any.h"  // Test double inclusion

#include "doctest.h"
#include <string>
#include <utility>
#include <vector>
#include <type_traits>

#define ANY_TEST_00_INIT
#define ANY_TEST_01_OWNERSHIP
#define ANY_TEST_02_TYPE
#define ANY_TEST_03_SWAP
#define ANY_TEST_04_MOVABLE
#define ANY_TEST_05_COPYABLE
#define ANY_TEST_06_ANY_CAST_NON_CONST
#define ANY_TEST_07_ANY_CAST_CONST
#define ANY_TEST_08_ANY_CAST_VAL
#define ANY_TEST_09_ANY_CAST_RREF
#define ANY_TEST_10_ANY_CAST_LREF

#ifdef ANY_TEST_00_INIT
TEST_CASE("Default-initialize any") {
    cls_26::any x;
    static_cast<void>(x);
}

TEST_CASE("Copy-initialize any from...") {
    SUBCASE("int") {
        cls_26::any x = 10;
        static_cast<void>(x);
    }
    SUBCASE("std::string") {
        cls_26::any x = std::string("foo");
        static_cast<void>(x);
    }
    SUBCASE("std::vector<int>") {
        cls_26::any x = std::vector{1, 2, 3};
        static_cast<void>(x);
    }
}
#endif  // ANY_TEST_00_COPY_INIT

#ifdef ANY_TEST_01_OWNERSHIP
TEST_CASE("any owns the object") {
    int destructors = 0;
    struct DestructCounter {
        int *destructorsCounter;

        ~DestructCounter() { ++*destructorsCounter; }
    };

    {
        DestructCounter counter{&destructors};
        REQUIRE(destructors == 0);
        {
            cls_26::any x = counter;
            static_cast<void>(x);
            CHECK(destructors == 0);
        }
        CHECK(destructors == 1);
    }
    CHECK(destructors == 2);
}
#endif  // ANY_TEST_01_ONWERSHIP

#ifdef ANY_TEST_02_TYPE
TEST_CASE("any knows type of the object") {
    SUBCASE("int") {
        const cls_26::any &x = 10;
        CHECK(x.type() == typeid(int));
    }
    SUBCASE("std::string") {
        const cls_26::any &x = std::string("foo");
        CHECK(x.type() == typeid(std::string));
    }
    SUBCASE("std::vector<int>") {
        const cls_26::any &x = std::vector{1, 2, 3};
        CHECK(x.type() == typeid(std::vector<int>));
    }

    struct Base { virtual ~Base() {} };
    struct Derived : Base {};

    SUBCASE("Polymorphic Derived*") {
        auto data = std::make_unique<Derived>();
        const cls_26::any &x = data.get();
        CHECK(x.type() == typeid(Derived*));
    }
    SUBCASE("Polymorphic Base*") {
        auto data = std::make_unique<Derived>();
        const cls_26::any &x = static_cast<Base*>(data.get());
        CHECK(x.type() == typeid(Base*));
    }

    SUBCASE("empty any") {
        cls_26::any x;
        CHECK(x.type() == typeid(void));
    }
}
#endif  // ANY_TEST_02_TYPE

#ifdef ANY_TEST_03_SWAP
TEST_CASE("std::swap<any> works") {
    cls_26::any a = 10;
    cls_26::any b = std::string("foo");
    std::swap(a, b);
    CHECK(a.type() == typeid(std::string));
    CHECK(b.type() == typeid(int));
}
#endif  // ANY_TEST_03_SWAP

#ifdef ANY_TEST_04_MOVABLE
TEST_CASE("any is move-constructible") {
    cls_26::any a = 10;
    cls_26::any b(std::move(a));
    CHECK(b.type() == typeid(int));
    CHECK(a.type() == typeid(void));

    cls_26::any c(std::move(a));
    CHECK(c.type() == typeid(void));
    CHECK(a.type() == typeid(void));
}

TEST_CASE("any is move-assignable") {
    cls_26::any a = 10;
    cls_26::any b = std::string("foo");
    cls_26::any c;
    cls_26::any d;

    a = std::move(b);  // non-empty <-- non-empty
    CHECK(a.type() == typeid(std::string));
    CHECK(b.type() == typeid(void));

    b = std::move(a);  // empty <-- non-empty
    CHECK(b.type() == typeid(std::string));
    CHECK(a.type() == typeid(void));

    c = std::move(a);  // empty <-- empty
    CHECK(c.type() == typeid(void));
    CHECK(a.type() == typeid(void));

    b = std::move(a);  // non-empty <-- empty
    CHECK(b.type() == typeid(void));
    CHECK(a.type() == typeid(void));

    cls_26::any &bres = b = std::move(a);
    CHECK(&bres == &b);
}
#endif  // ANY_TEST_04_MOVABLE

#ifdef ANY_TEST_05_COPYABLE
TEST_CASE("any is copy-constructible") {
    SUBCASE("") {
        cls_26::any a;
        cls_26::any b = a;
        CHECK(a.type() == typeid(void));
        CHECK(b.type() == typeid(void));
    }

    SUBCASE("any with data") {
        std::vector<int> data(1'000'000);
        cls_26::any a = data;
        REQUIRE(a.type() == typeid(std::vector<int>));
        cls_26::any b = a;
        CHECK(a.type() == typeid(std::vector<int>));
        CHECK(b.type() == typeid(std::vector<int>));
    }
}

TEST_CASE("any is copy-constructible") {
    cls_26::any a = 10;
    cls_26::any b = std::string("foo");
    cls_26::any c;
    cls_26::any d;

    a = b;  // non-empty <-- non-empty
    CHECK(a.type() == typeid(std::string));
    CHECK(b.type() == typeid(std::string));

    c = d;  // empty <-- empty
    CHECK(c.type() == typeid(void));
    CHECK(d.type() == typeid(void));

    c = b;  // empty <-- non-empty
    CHECK(c.type() == typeid(std::string));
    CHECK(b.type() == typeid(std::string));

    b = d;  // non-empty <-- empty
    CHECK(b.type() == typeid(void));
    CHECK(d.type() == typeid(void));

    cls_26::any &bres = b = d;
    CHECK(&bres == &b);

    a = 12345;  // Implicit conversion.
    CHECK(a.type() == typeid(int));
}
#endif  // ANY_TEST_05_COPYABLE

#ifdef ANY_TEST_06_ANY_CAST_NON_CONST
TEST_CASE("any_cast<>(any*) works") {
    SUBCASE("nullptr to int") {
        cls_26::any *a = nullptr;
        CHECK(cls_26::any_cast<int>(a) == nullptr);
    }
    SUBCASE("nullptr to std::string") {
        cls_26::any *a = nullptr;
        CHECK(cls_26::any_cast<std::string>(a) == nullptr);
    }
    SUBCASE("nullptr to void") {
        cls_26::any *a = nullptr;
        CHECK(cls_26::any_cast<void>(a) == nullptr);
    }

    SUBCASE("empty any to int") {
        cls_26::any a;
        CHECK(cls_26::any_cast<int>(&a) == nullptr);
    }
    SUBCASE("empty any to std::string") {
        cls_26::any a;
        CHECK(cls_26::any_cast<std::string>(&a) == nullptr);
    }
    SUBCASE("empty any to void") {
        cls_26::any a;
        CHECK(cls_26::any_cast<void>(&a) == nullptr);
    }

    SUBCASE("any to int") {
        cls_26::any a = 10;
        static_assert(std::is_same_v<int*, decltype(cls_26::any_cast<int>(&a))>);
        int *ptr = cls_26::any_cast<int>(&a);
        REQUIRE(ptr);
        CHECK(*ptr == 10);
        *ptr = 20;
        CHECK(*ptr == 20);

        int *ptr2 = cls_26::any_cast<int>(&a);
        CHECK(ptr2 == ptr);
    }
    SUBCASE("any to std::string") {
        cls_26::any a = std::string("foo");
        static_assert(std::is_same_v<std::string*, decltype(cls_26::any_cast<std::string>(&a))>);
        std::string *ptr = cls_26::any_cast<std::string>(&a);
        REQUIRE(ptr);
        CHECK(*ptr == "foo");
        *ptr += "x";
        CHECK(*ptr == "foox");

        std::string *ptr2 = cls_26::any_cast<std::string>(&a);
        CHECK(ptr2 == ptr);
    }

    SUBCASE("any to int erroneously") {
        cls_26::any a = std::string("foo");
        CHECK(cls_26::any_cast<int>(&a) == nullptr);
    }
    SUBCASE("any to std::string erroneously") {
        cls_26::any a = 10;
        CHECK(cls_26::any_cast<std::string>(&a) == nullptr);
    }
}
#endif  // ANY_TEST_06_ANY_CAST_NON_CONST

#ifdef ANY_TEST_07_ANY_CAST_CONST
TEST_CASE("any_cast<>(any*) works") {
    SUBCASE("nullptr to different types") {
        const cls_26::any *a = nullptr;
        CHECK(cls_26::any_cast<int>(a) == nullptr);
        CHECK(cls_26::any_cast<std::string>(a) == nullptr);
    }
    SUBCASE("nullptr to void") {
        const cls_26::any *a = nullptr;
        CHECK(cls_26::any_cast<void>(a) == nullptr);
    }

    SUBCASE("empty any to different types") {
        const cls_26::any a;
        CHECK(cls_26::any_cast<int>(&a) == nullptr);
        CHECK(cls_26::any_cast<std::string>(&a) == nullptr);
    }
    SUBCASE("empty any to void") {
        const cls_26::any a;
        CHECK(cls_26::any_cast<void>(&a) == nullptr);
    }

    SUBCASE("any to int") {
        const cls_26::any a = 10;
        static_assert(std::is_same_v<const int*, decltype(cls_26::any_cast<int>(&a))>);
        const int *ptr = cls_26::any_cast<int>(&a);
        REQUIRE(ptr);
        CHECK(*ptr == 10);

        const int *ptr2 = cls_26::any_cast<int>(&a);
        CHECK(ptr2 == ptr);
    }
    SUBCASE("any to std::string") {
        const cls_26::any a = std::string("foo");
        static_assert(std::is_same_v<const std::string*, decltype(cls_26::any_cast<std::string>(&a))>);
        const std::string *ptr = cls_26::any_cast<std::string>(&a);
        REQUIRE(ptr);
        CHECK(*ptr == "foo");

        const std::string *ptr2 = cls_26::any_cast<std::string>(&a);
        CHECK(ptr2 == ptr);
    }

    SUBCASE("any to int erroneously") {
        const cls_26::any a = std::string("foo");
        CHECK(cls_26::any_cast<int>(&a) == nullptr);
    }
    SUBCASE("any to std::string erroneously") {
        const cls_26::any a = 10;
        CHECK(cls_26::any_cast<std::string>(&a) == nullptr);
    }
}
#endif  // ANY_TEST_07_ANY_CAST_CONST

#ifdef ANY_TEST_08_ANY_CAST_VAL
static_assert(std::is_convertible_v<cls_26::bad_any_cast&, std::logic_error&>);

TEST_CASE("any_cast<>(any&) and any_cast<>(const any&) work") {
    SUBCASE("any to int") {
        cls_26::any a = 10;
        static_assert(!std::is_reference_v<decltype(cls_26::any_cast<int>(a))>);
        static_assert(!std::is_const_v<decltype(cls_26::any_cast<int>(a))>);
        CHECK(cls_26::any_cast<int>(a) == 10);
    }
    SUBCASE("any to std::string") {
        cls_26::any a = std::string("foo");
        static_assert(!std::is_reference_v<decltype(cls_26::any_cast<std::string>(a))>);
        static_assert(!std::is_const_v<decltype(cls_26::any_cast<std::string>(a))>);
        CHECK(cls_26::any_cast<std::string>(a) == "foo");
    }
    SUBCASE("const any to int") {
        const cls_26::any a = 10;
        static_assert(!std::is_reference_v<decltype(cls_26::any_cast<int>(a))>);
        static_assert(!std::is_const_v<decltype(cls_26::any_cast<int>(a))>);
        CHECK(cls_26::any_cast<int>(a) == 10);
    }
    SUBCASE("const any to std::string") {
        const cls_26::any a = std::string("foo");
        static_assert(!std::is_reference_v<decltype(cls_26::any_cast<std::string>(a))>);
        static_assert(!std::is_const_v<decltype(cls_26::any_cast<std::string>(a))>);
        CHECK(cls_26::any_cast<std::string>(a) == "foo");
    }

    SUBCASE("empty any to different_types") {
        cls_26::any a;
        CHECK_THROWS_AS(cls_26::any_cast<int>(a), const cls_26::bad_any_cast &);
        CHECK_THROWS_AS(cls_26::any_cast<std::string>(a), const cls_26::bad_any_cast &);
    }
    SUBCASE("const empty any to different_types") {
        const cls_26::any a;
        CHECK_THROWS_AS(cls_26::any_cast<int>(a), const cls_26::bad_any_cast &);
        CHECK_THROWS_AS(cls_26::any_cast<std::string>(a), const cls_26::bad_any_cast &);
    }

    SUBCASE("any to int erroneously") {
        cls_26::any a = std::string("foo");
        CHECK_THROWS_AS(cls_26::any_cast<int>(a), const cls_26::bad_any_cast &);
    }
    SUBCASE("any to std::string erroneously") {
        cls_26::any a = 10;
        CHECK_THROWS_AS(cls_26::any_cast<std::string>(a), const cls_26::bad_any_cast &);
    }
    SUBCASE("const any to int erroneously") {
        cls_26::any a = std::string("foo");
        CHECK_THROWS_AS(cls_26::any_cast<int>(a), const cls_26::bad_any_cast &);
    }
    SUBCASE("const any to std::string erroneously") {
        cls_26::any a = 10;
        CHECK_THROWS_AS(cls_26::any_cast<std::string>(a), const cls_26::bad_any_cast &);
    }
}
#endif  // ANY_TEST_08_ANY_CAST_VAL

#ifdef ANY_TEST_09_ANY_CAST_RREF
TEST_CASE("any_cast<T>(any&&) works and moves from") {
    struct MovableFrom {
        bool moved = false, moved_from = false;

        MovableFrom() {}
        MovableFrom(const MovableFrom &other) : moved(other.moved), moved_from(other.moved_from) {}
        MovableFrom(MovableFrom &&other) : moved(true) { other.moved_from = true; }
    };

    MovableFrom object;
    REQUIRE(!object.moved_from);
    cls_26::any a = object;
    CHECK(!object.moved_from);

    static_assert(!std::is_reference_v<decltype(cls_26::any_cast<MovableFrom>(std::move(a)))>);
    static_assert(!std::is_const_v<decltype(cls_26::any_cast<MovableFrom>(std::move(a)))>);

    SUBCASE("check moves") {
        {
            MovableFrom inside = cls_26::any_cast<MovableFrom>(a);
            CHECK(!inside.moved);
            CHECK(!inside.moved_from);
        }
        {
            MovableFrom inside = cls_26::any_cast<MovableFrom>(a);
            CHECK(!inside.moved);
            CHECK(!inside.moved_from);
        }
        {
            MovableFrom inside = cls_26::any_cast<MovableFrom>(std::move(a));
            CHECK(inside.moved);
            CHECK(!inside.moved_from);
        }
        {
            MovableFrom inside = cls_26::any_cast<MovableFrom>(a);
            CHECK(!inside.moved);
            CHECK(inside.moved_from);
        }
    }

    SUBCASE("exception is raised on erroneous conversion") {
        CHECK_THROWS_AS(cls_26::any_cast<std::string>(a), const cls_26::bad_any_cast &);
    }
}
#endif  // ANY_TEST_09_ANY_CAST_REF

#ifdef ANY_TEST_10_ANY_CAST_LREF
TEST_CASE("any_cast<T&>(any)") {
    SUBCASE("any to int&") {
        cls_26::any a = 10;
        int &x = cls_26::any_cast<int&>(a);
        REQUIRE(x == 10);
        x = 20;
        REQUIRE(x == 20);
        REQUIRE(cls_26::any_cast<int>(a) == 20);
    }
    SUBCASE("any to std::string&") {
        cls_26::any a = std::string("foo");
        std::string &x = cls_26::any_cast<std::string&>(a);
        REQUIRE(x == "foo");
        x += "x";
        REQUIRE(x == "foox");
        REQUIRE(cls_26::any_cast<std::string>(a) == "foox");
    }
    SUBCASE("exception is raised on erroneous conversion") {
        cls_26::any a = 10;
        CHECK_THROWS_AS(cls_26::any_cast<std::string&>(a), const cls_26::bad_any_cast &);
    }
}
#endif  // ANY_TEST_10_ANY_CAST_LREF
