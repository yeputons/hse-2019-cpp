#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <string>

using namespace std;

int main() {
    vector<string> strs;
    // Считайте из cin несколько строчек, отсортируйте, выведите в cout на разных строках.
    istream_iterator<string> inBegin(cin), inEnd;
    copy(inBegin, inEnd, back_inserter(strs));
    sort(strs.begin(), strs.end());
    copy(strs.begin(), strs.end(), ostream_iterator<string>(cout, "\n"));
}
