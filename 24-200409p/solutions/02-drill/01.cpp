#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <string>

int main() {
    std::vector<std::string> strs;
    // Считайте из cin несколько строчек, отсортируйте, выведите в cout на разных строках.
    std::istream_iterator<std::string> inBegin(std::cin), inEnd;
    std::copy(inBegin, inEnd, std::back_inserter(strs));
    std::sort(strs.begin(), strs.end());
    std::copy(strs.begin(), strs.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
}
