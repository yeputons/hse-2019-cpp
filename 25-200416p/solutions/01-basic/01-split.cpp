#include <cassert>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

using std::string_view;
using std::vector;

// START SOLUTION: implement split()
vector<string_view> split(string_view str, string_view sep = " ") {
    vector<string_view> result;
    while (true) {
        auto pos = str.find(sep);
        result.emplace_back(str.substr(0, pos));
        if (pos == string_view::npos) break;
        str.remove_prefix(pos + sep.size());
    }
    return result;
}
// END SOLUTION

int main() {
    using std::string;
    using vecsv = vector<string_view>;

    assert(split("") == vecsv{""});
    assert(split("hello world") == (vecsv{"hello", "world"}));
    assert(split(" hello world") == (vecsv{"", "hello", "world"}));
    assert(split("  hello world") == (vecsv{"", "", "hello", "world"}));
    assert(split("hello world ") == (vecsv{"hello", "world", ""}));
    assert(split("hello world  ") == (vecsv{"hello", "world", "", ""}));
    assert(split("  hello  world  ") == (vecsv{"", "", "hello", "", "world", "", ""}));

    assert(split("hello world", ",") == (vecsv{"hello world"}));
    assert(split("hello, world", ",") == (vecsv{"hello", " world"}));

    assert(split("hello, world", ", ") == (vecsv{"hello", "world"}));
    assert(split("he,llo, world", ", ") == (vecsv{"he,llo", "world"}));

    {
        string s = "hello world";
        vecsv v = split(s);
        assert(v == (vecsv{"hello", "world"}));
    }
    {
        string s = "hello, world", sep = ", ";
        vecsv v = split(s, sep);
        assert(v == (vecsv{"hello", "world"}));
    }
    {
        string sep = ", ";
        vecsv v = split("hello, world", sep);
        assert(v == (vecsv{"hello", "world"}));
    }
    std::cout << "PASSED\n";
}
