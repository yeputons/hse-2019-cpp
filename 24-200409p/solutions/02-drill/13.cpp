#include <algorithm>
#include <iostream>
#include <iterator>
#include <unordered_set>

int main() {
    std::vector<int> a = {1, 2, 3, 4, 2, 5, 4, 1, 10};
    // Для каждого дублирующегося элемента удалите все копии, начиная со второй.
    // Время работы должно быть в среднем линейно, разрешается создать промежуточный контейнер.
    // Выведите результат на экран.
    a.erase(std::remove_if(
        a.begin(), a.end(),
        [found = std::unordered_set<int>()](int x) mutable {
            // not static variable!
            return !found.insert(x).second;
        }
    ), a.end());
    std::copy(a.begin(), a.end(), std::ostream_iterator<int>(std::cout, "\n"));
}
