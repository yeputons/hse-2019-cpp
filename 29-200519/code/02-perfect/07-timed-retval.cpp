#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <utility>
#include <vector>

bool &calculate(int n, bool &completed) {
    const int inf = 1'000'000'000;
    std::vector<std::vector<int>> graph(n, std::vector<int>(n, inf));
    std::mt19937 gen;
    std::uniform_int_distribution distrib(0, 100000);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            graph[i][j] = distrib(gen);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++)
                graph[j][k] = std::min(graph[j][i], graph[i][k]);
    return completed = true;
}

template<typename Fn, typename Arg0, typename Arg1>
decltype(auto) timed(Fn fn, Arg0&& arg0, Arg1&& arg1) {
    struct Timer {
        std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
        ~Timer() {
            auto duration = std::chrono::steady_clock::now() - start;
            std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms\n";
        }
    } timer;
    return fn(static_cast<Arg0&&>(arg0), static_cast<Arg1&&>(arg1));  // std::forward<Arg0>(arg0)
}

int main() {
    bool completed = false;
    try {
        bool &completed2 = timed(calculate, 100, completed);
        std::cout << completed << "\n";
        std::cout << &completed << " == " << &completed2 << "\n";
    } catch (...) {
        std::cout << "exception\n";
    }
}
