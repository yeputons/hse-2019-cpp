#include <cassert>
#include <iostream>
#include <unordered_set>

using std::cout;
using std::unordered_set;

struct point {
    int x = 0, y = 0;
    point() {}
    point(int x_, int y_) : x(x_), y(y_) {}
    bool operator==(const point &other) const {
        return x == other.x && y == other.y;
    }
};

struct PointHasher {
    size_t operator()(const point &p) const {
        return std::hash<int>()(p.x) * 239017 + std::hash<int>()(p.y);
    }
};

int main() {
    {
        unordered_set<int> values = {1, 2, 3, 4, 5};
        for (int v : values) cout << v << "\n";  // Порядок неизвестен.
    }

    {
        unordered_set<point, PointHasher> points;
        points.emplace(10, 20);
        points.emplace(20, 10);
        points.emplace(10, 20);
        assert(points.size() == 2);
    }
}
