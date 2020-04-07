#include <cassert>
#include <map>
#include <string>
#include <iostream>
#include <iterator>

using std::map;
using std::string;
using std::pair;

int main() {
    map<string, int> m = {
        {"key", 10},
        {"foo", 20}
    };
    assert(m.size() == 2);
    assert(m.count("key") == 1);
    assert(m["key"] == 10);	

    if (m["bar"]) {}
    assert((m == map<string, int>{{"bar", 0}, {"foo", 20}, {"key", 10}}));
    assert(m["wtf"] == 0);  // Работает!

    {
        map<string, int>::iterator it = m.find("foo");
        assert(it == std::next(m.begin()));
        assert(it->first == "foo");
        assert(it->second == 20);
        // it->first += "x";  // Нельзя менять ключ.
        it->second += 3;  // Значение менять можно.
    }
    assert(m.emplace("super", 33).second);
    assert(m["super"] == 33);
}
