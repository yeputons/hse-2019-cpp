#include <bitset>
#include <cassert>
#include <iostream>

using std::bitset;

int main() {
    bitset<10> bs(   "0000010011");
    bs.set(2);     // 0000010111
    assert(bs.test(2));
    assert(bs[2]);
    bs.reset(2);   // 0000010011
    bs.flip();     // 1111101100
    assert(bs.count() == 7);
    bs ^= bitset<10>("1100000001");
                   // 0011101101
    assert(bs.count() == 6);
    std::cout << bs << "\n";
}
