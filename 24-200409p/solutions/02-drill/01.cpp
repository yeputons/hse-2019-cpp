#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>

int main() {
    std::vector<std::string> strs;
    std::istream_iterator<std::string> inBegin(std::cin), inEnd;
    std::copy(inBegin, inEnd, std::back_inserter(strs));
    std::sort(strs.begin(), strs.end());
    std::copy(strs.begin(), strs.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
}
