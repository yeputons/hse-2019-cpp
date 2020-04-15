#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iostream>
#include <vector>
#include <utility>
#include <type_traits>

using std::initializer_list;
using std::sort;
using std::unique;

template<typename T>
struct sort_unique_vector {
    sort_unique_vector() {}

    explicit sort_unique_vector(initializer_list<T> data_) : data(data_) {  // Direct initialization. No move needed for light initializer_list<>.
        sort(data.begin(), data.end());
        data.erase(unique(data.begin(), data.end()), data.end());
    }

    auto begin() const { return data.cbegin(); }
    auto end() const { return data.cend(); }
    const T& operator[](typename std::vector<T>::size_type i) const {
        return data[i];
    }

private:
    std::vector<T> data;
};

int main() {
    using std::vector;
    {
        sort_unique_vector<int> suvec;
        assert(vector(suvec.begin(), suvec.end()).empty());
    }
    {
        sort_unique_vector<int> suvec{3, 2, 1, 2, 3, 4, 5, 4, 1, 2, 2};
        assert(vector(suvec.begin(), suvec.end()) == (vector{1, 2, 3, 4, 5}));
        #if 0
        *suvec.begin() = 0;  // Should be compilation error.
        suvec[0] = 0;  // Should be compilation error.
        #endif
    }
    {
        sort_unique_vector suvec{3, 2, 1, 2, 3, 4, 5, 4, 1, 2, 2};
        assert(vector(suvec.begin(), suvec.end()) == (vector{1, 2, 3, 4, 5}));
    }
    {
        sort_unique_vector suvec{3.0, 2.5, 2.5, 3.5};
        assert(vector(suvec.begin(), suvec.end()) == (vector{2.5, 3.0, 3.5}));
    }
    {
        #if 0
        sort_unique_vector<int> suvec{1.0};  // Should be compilation error without any special handling.
        #endif
    }
    {
        #if 0
        sort_unique_vector<int> suvec = {3, 2, 1, 2, 3, 4, 5, 4, 1, 2, 2};  // Should be compilation error.
        #endif
    }
    {
        // Copy should be automatically supported.
        sort_unique_vector suvec{1, 2, 3, 4, 5, 1, 2, 3, 4, 5};
        sort_unique_vector suvec2 = suvec;
        assert(vector(suvec.begin(), suvec.end()) == (vector{1, 2, 3, 4, 5}));
        assert(vector(suvec2.begin(), suvec2.end()) == (vector{1, 2, 3, 4, 5}));
    }
    {
        // Move should be automatically supported.
        sort_unique_vector suvec{1, 2, 3, 4, 5, 1, 2, 3, 4, 5};
        sort_unique_vector suvec2 = std::move(suvec);
        // assert(vector(suvec.begin(), suvec.end()).empty()); // Not always true.
        assert(vector(suvec2.begin(), suvec2.end()) == (vector{1, 2, 3, 4, 5}));
    }
    std::cout << "PASSED\n";
}
