#include <cassert>
#include <iostream>
#include <map>
#include <tuple>

using std::cout;
using std::map;
using std::string;

int main() {
    {
        std::pair<int, string> p(10, "foo");
        {
            auto [a, b] = p;  // a == 10, b == "foo"
            assert(a == 10);
            assert(b == "foo");
            b += "x";  // b == "foox", p.second == "foo"
            assert(b == "foox");
            assert(p.second == "foo");
        }
        {
            auto& [a, b] = p;  // a == 10, b == "foo"
            assert(a == 10);
            assert(b == "foo");
            b += "x";  // b == p.second == "foox"
            assert(b == "foox");
            assert(p.second == "foox");
        }
    }
    {
        #if 1  // Alternate between 1 and 0.
        map<int, string> m = {{1, "bar"}, {2, "baz"}};
        #else
        map<int, string> m = {{1, "bar"}, {10, "baz"}};
        #endif
        if (auto [it, inserted] = m.emplace(10, "foo"); inserted) {
            cout << "Inserted, value is " << it->second << '\n';
        } else {
            cout << "Already exists, value is " << it->second << '\n';
        }
        for (const auto &[key, value] : m) {
            cout << key << ": " << value << '\n';
        }
    }
}
