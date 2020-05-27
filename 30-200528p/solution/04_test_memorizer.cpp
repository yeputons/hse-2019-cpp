#include "04_memorizer.h"
#include "04_memorizer.h"  // Test include guards.

#include "doctest.h"
#include <sstream>
#include <string>
#include <utility>

#define TEST_MEMORIZER_01_CALL_LOG
#define TEST_MEMORIZER_02_MEMORIZER_SPECIFIED
#define TEST_MEMORIZER_03_MEMORIZER_COPIES_ARGS
#define TEST_MEMORIZER_04_FUNCTION_TRAITS
#define TEST_MEMORIZER_05_MEMORIZER_DEDUCTION
#define TEST_MEMORIZER_06_MEMORIZER_DEDUCTION_NO_REFS
#define TEST_MEMORIZER_07_MEMORIZER_SPECIFIED_ARG_REFS
#define TEST_MEMORIZER_08_MEMORIZER_SPECIFIED_RET_REF
#define TEST_MEMORIZER_09_MEMORIZER_SPECIFIED_RET_VOID
#define TEST_MEMORIZER_10_MEMORIZER_DEDUCTION_ALL

int sum(int a, int b) {
    return a + b;
}

int sumCvrefArgs(const int &a, const int &b) {
    return a + b;
}

int& increase(int &a, const int &b) {
    return a += b;
}

void increaseNoReturn(int &a, int b) {
    a += b;
}

#ifdef TEST_MEMORIZER_01_CALL_LOG
TEST_CASE("CallLog can be constructed") {
    cls_30::CallLog<int, std::string, double> log(10, "hello", 10.5);
    CHECK(log.result == 10);
    auto [arg0, arg1] = log.arguments;
    CHECK(arg0 == "hello");
    CHECK(arg1 == 10.5);
}
#endif  // TEST_MEMORIZER_01_CALL_LOG

#ifdef TEST_MEMORIZER_02_MEMORIZER_SPECIFIED
TEST_CASE("Memorizer<Fn, C> remembers calls") {
    cls_30::Memorizer<int(*)(int, int), cls_30::CallLog<int, int, int>> sumMem(sum);
    CHECK(sumMem(10, 20) == 30);
    CHECK(sumMem(100, 200) == 300);

    // Verbose checks below are for better error reporting,
    // as CallLog is neither printable nor comparable.
    REQUIRE(sumMem.calls.size() == 2);
    {
        CHECK(sumMem.calls[0].result == 30);
        auto [arg0, arg1] = sumMem.calls[0].arguments;
        CHECK(arg0 == 10);
        CHECK(arg1 == 20);
    }
    {
        CHECK(sumMem.calls[1].result == 300);
        auto [arg0, arg1] = sumMem.calls[1].arguments;
        CHECK(arg0 == 100);
        CHECK(arg1 == 200);
    }
}
#endif  // TEST_MEMORIZER_02_MEMORIZER_SPECIFIED

#ifdef TEST_MEMORIZER_03_MEMORIZER_COPIES_ARGS
struct MoveTracker {
    bool moved_from = false;
    MoveTracker(const MoveTracker&) = default;
    MoveTracker& operator=(const MoveTracker&) = default;
    MoveTracker() {}
    MoveTracker(MoveTracker &&other) : moved_from(other.moved_from) { other.moved_from = true; }
    MoveTracker& operator=(MoveTracker &&other) {
        moved_from = other.moved_from;
        other.moved_from = true;
        return *this;
    }
};
MoveTracker ensure_unmoved(MoveTracker tracker) {
    CHECK(!tracker.moved_from);
    return MoveTracker();
}
TEST_CASE("Memorizer<Fn, C> copies arguments even when they are movable") {
    cls_30::Memorizer<MoveTracker(*)(MoveTracker), cls_30::CallLog<MoveTracker, MoveTracker>> mem(ensure_unmoved);
    MoveTracker result = mem(MoveTracker());
    CHECK(!result.moved_from);
    REQUIRE(mem.calls.size() == 1);
    {
        CHECK(!mem.calls[0].result.moved_from);
        const auto& [arg0] = mem.calls[0].arguments;
        CHECK(!arg0.moved_from);
    }
}
#endif  // TEST_MEMORIZER_03_MEMORIZER_COPIES_ARGS

#ifdef TEST_MEMORIZER_04_FUNCTION_TRAITS
TEST_CASE("impl::function_traits yields correct call_result_type") {
    CHECK(std::is_same_v<
        cls_30::impl::function_traits<int(*)()>::call_result_type,
        cls_30::CallLog<int>
    >);
    CHECK(std::is_same_v<
        cls_30::impl::function_traits<int(*)(std::string, char)>::call_result_type,
        cls_30::CallLog<int, std::string, char>
    >);
}
#endif  // TEST_MEMORIZER_04_FUNCTION_TRAITS

#ifdef TEST_MEMORIZER_05_MEMORIZER_DEDUCTION
TEST_CASE("Memorizer automatically deduces its arguments") {
    cls_30::Memorizer sumMem(sum);
    CHECK(sumMem(10, 20) == 30);
    CHECK(sumMem(100, 200) == 300);

    REQUIRE(sumMem.calls.size() == 2);
    {
        CHECK(sumMem.calls[0].result == 30);
        auto [arg0, arg1] = sumMem.calls[0].arguments;
        CHECK(arg0 == 10);
        CHECK(arg1 == 20);
    }
    {
        CHECK(sumMem.calls[1].result == 300);
        auto [arg0, arg1] = sumMem.calls[1].arguments;
        CHECK(arg0 == 100);
        CHECK(arg1 == 200);
    }
}
#endif  // TEST_MEMORIZER_05_MEMORIZER_DEDUCTION

#ifdef TEST_MEMORIZER_06_MEMORIZER_DEDUCTION_NO_REFS
TEST_CASE("Memorizer automatically deduces its arguments and stores them by-value") {
    cls_30::Memorizer sumCvrefArgsMem(sumCvrefArgs);
    CHECK(sumCvrefArgsMem(10, 20) == 30);
    CHECK(sumCvrefArgsMem(100, 200) == 300);

    REQUIRE(sumCvrefArgsMem.calls.size() == 2);
    {
        CHECK(sumCvrefArgsMem.calls[0].result == 30);
        auto [arg0, arg1] = sumCvrefArgsMem.calls[0].arguments;
        CHECK(arg0 == 10);
        CHECK(arg1 == 20);
    }
    {
        CHECK(sumCvrefArgsMem.calls[1].result == 300);
        auto [arg0, arg1] = sumCvrefArgsMem.calls[1].arguments;
        CHECK(arg0 == 100);
        CHECK(arg1 == 200);
    }
}
#endif  // TEST_MEMORIZER_06_MEMORIZER_DEDUCTION_NO_REFS

#ifdef TEST_MEMORIZER_07_MEMORIZER_SPECIFIED_ARG_REFS
TEST_CASE("Memorizer<Fn, C> allows lvalue refs in arguments") {
    cls_30::Memorizer<int&(*)(int&, const int&), cls_30::CallLog<int, int, int>> increaseMem(increase);
    int value = 0;
    CHECK(increaseMem(value, 10) == 10);
    CHECK(value == 10);
    CHECK(increaseMem(value, 20) == 30);
    CHECK(value == 30);
}

TEST_CASE("Memorizer<Fn, C> stores lvalue refs values before call") {
    cls_30::Memorizer<int &(*)(int &, const int &), cls_30::CallLog<int, int, int>> increaseMem(increase);
    int value = 0;
    CHECK(increaseMem(value, 10) == 10);
    CHECK(value == 10);
    CHECK(increaseMem(value, 20) == 30);
    CHECK(value == 30);

    REQUIRE(increaseMem.calls.size() == 2);
    {
        CHECK(increaseMem.calls[0].result == 10);
        auto[arg0, arg1] = increaseMem.calls[0].arguments;
        CHECK(arg0 == 0);
        CHECK(arg1 == 10);
    }
    {
        CHECK(increaseMem.calls[1].result == 30);
        auto[arg0, arg1] = increaseMem.calls[1].arguments;
        CHECK(arg0 == 10);
        CHECK(arg1 == 20);
    }
}
#endif  // TEST_MEMORIZER_07_MEMORIZER_SPECIFIED_ARG_REFS

#ifdef TEST_MEMORIZER_08_MEMORIZER_SPECIFIED_RET_REF
TEST_CASE("Memorizer<Fn, C> correctly returns references") {
    cls_30::Memorizer<int&(*)(int&, const int&), cls_30::CallLog<int, int, int>> increaseMem(increase);
    int value = 0;
    CHECK(&increaseMem(value, 10) == &value);
    CHECK(value == 10);
    CHECK(&increaseMem(value, 20) == &value);
    CHECK(value == 30);
}
#endif  // TEST_MEMORIZER_08_MEMORIZER_SPECIFIED_RET_REF

#ifdef TEST_MEMORIZER_09_MEMORIZER_SPECIFIED_RET_VOID
TEST_CASE("CallLog<void> can be constructed") {
    cls_30::CallLog<void, std::string, double> log("hello", 10.5);
    auto [arg0, arg1] = log.arguments;
    CHECK(arg0 == "hello");
    CHECK(arg1 == 10.5);
}

TEST_CASE("Memorizer<Fn, C> correctly works with functions returning void") {
    cls_30::Memorizer<void(*)(int&, int), cls_30::CallLog<void, int, int>> increaseNoReturnMem(increaseNoReturn);
    int value = 0;
    increaseNoReturnMem(value, 10);
    CHECK(value == 10);
    increaseNoReturnMem(value, 20);
    CHECK(value == 30);

    REQUIRE(increaseNoReturnMem.calls.size() == 2);
    {
        auto[arg0, arg1] = increaseNoReturnMem.calls[0].arguments;
        CHECK(arg0 == 0);
        CHECK(arg1 == 10);
    }
    {
        auto[arg0, arg1] = increaseNoReturnMem.calls[1].arguments;
        CHECK(arg0 == 10);
        CHECK(arg1 == 20);
    }
}
#endif  // TEST_MEMORIZER_09_MEMORIZER_SPECIFIED_RET_VOID

#ifdef TEST_MEMORIZER_10_MEMORIZER_DEDUCTION_ALL
TEST_CASE("Memorizer deduces complex arguments/return values correctly") {
    cls_30::Memorizer increaseMem(increase);
    int value = 0;
    CHECK(&increaseMem(value, 10) == &value);
    CHECK(value == 10);
    CHECK(&increaseMem(value, 20) == &value);
    CHECK(value == 30);

    REQUIRE(increaseMem.calls.size() == 2);
    {
        CHECK(increaseMem.calls[0].result == 10);
        auto[arg0, arg1] = increaseMem.calls[0].arguments;
        CHECK(arg0 == 0);
        CHECK(arg1 == 10);
    }
    {
        CHECK(increaseMem.calls[1].result == 30);
        auto[arg0, arg1] = increaseMem.calls[1].arguments;
        CHECK(arg0 == 10);
        CHECK(arg1 == 20);
    }
}

TEST_CASE("Memorizer deduces complex arguments and void return value correctly") {
    cls_30::Memorizer increaseNoReturnMem(increaseNoReturn);
    int value = 0;
    increaseNoReturnMem(value, 10);
    CHECK(value == 10);
    increaseNoReturnMem(value, 20);
    CHECK(value == 30);

    REQUIRE(increaseNoReturnMem.calls.size() == 2);
    {
        auto[arg0, arg1] = increaseNoReturnMem.calls[0].arguments;
        CHECK(arg0 == 0);
        CHECK(arg1 == 10);
    }
    {
        auto[arg0, arg1] = increaseNoReturnMem.calls[1].arguments;
        CHECK(arg0 == 10);
        CHECK(arg1 == 20);
    }
}
#endif  // TEST_MEMORIZER_10_MEMORIZER_DEDUCTION_ALL
