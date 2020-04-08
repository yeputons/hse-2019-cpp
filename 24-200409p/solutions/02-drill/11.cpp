#include <algorithm>
#include <iostream>
#include <iterator>
#include <unordered_set>

using namespace std;

int main() {
    vector<int> a = {1, 2, 3, 4, 2, 5, 4, 1, 10};
    // Для каждого дублирующегося элемента удалите все копии, начиная со второй.
    // Время работы должно быть в среднем линейно, разрешается создать промежуточный контейнер.
    // Выведите результат на экран.
    a.erase(remove_if(
        a.begin(), a.end(),
        [found = unordered_set<int>()](int x) mutable {
            // not static variable!
            return !found.insert(x).second;
        }
    ), a.end());
    copy(a.begin(), a.end(), ostream_iterator<int>(cout, "\n"));
}
