#include "any.h"
#include "any.h"  // Test double inclusion

#include "doctest.h"
#include <string>
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
TEST_CASE("std::swap<any> is movable") {
    cls_26::any a = 10;
    cls_26::any b(std::move(a));
    CHECK(b.type() == typeid(int));
    CHECK(a.type() == typeid(void));
}
#endif  // ANY_TEST_04_MOVABLE

#ifdef ANY_TEST_05_COPYABLE
TEST_CASE("std::swap<any> is copyable") {
    SUBCASE("empty any") {
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
        CHECK(ptr2 == ptr2);
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
        CHECK(ptr2 == ptr2);
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
        CHECK(ptr2 == ptr2);
    }
    SUBCASE("any to std::string") {
        const cls_26::any a = std::string("foo");
        static_assert(std::is_same_v<const std::string*, decltype(cls_26::any_cast<std::string>(&a))>);
        const std::string *ptr = cls_26::any_cast<std::string>(&a);
        REQUIRE(ptr);
        CHECK(*ptr == "foo");

        const std::string *ptr2 = cls_26::any_cast<std::string>(&a);
        CHECK(ptr2 == ptr2);
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
