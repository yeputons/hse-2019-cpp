#include <iostream>
#include <vector>

using namespace std;

template<typename T> void print(const T &) { cout << "1\n"; }
template<> void print<bool>(const bool &) { cout << "2a\n"; }
void print(const bool &) { cout << "2b\n"; }
template<typename T> void print(const vector<T> &) { cout << "3\n"; }

int main() {
    int x;
    bool y;
    vector<int> z;

    print(x);  // 1
    print(y);  // 2b
    print(z);  // 3

    print<int>(x);  // 1
    print<bool>(y);  // 2a (sic!)
    print<vector<int>>(z);  // 1 (sic!)
    print<int>(z);  // 3
    return 0;
}
