#include <algorithm>
#include <memory>
#include <iostream>
#include <iterator>
#include <unordered_set>
#include <mutex>

// "Кэширующая фабрика".
// При запросе элемента создаёт его при помощи функтора Factory, запоминает
// внутри себя и возвращает умный указатель на этот элемент.
// При последующих запросах возвращается тот же элемент.
// Если в какой-то момент элемент внутри оказался ненужным (т.е. на него
// перестали ссылаться умные указатели снаружи фабрики), то он удаляется.
// В следующий раз будет пересоздан заново.
//
// Гарантируется, что методы `CachingFactory` вызываются потокобезопасно.
// Однако объекты могут удаляться в других потоках в любой момент, в том
// числе одновременно с использованием `CachingFactory`.
template<typename Key, typename T, typename Factory>
struct CachingFactory {
    CachingFactory(Factory f_) : f(f_) {}

    // START SOLUTION
    std::shared_ptr<T> operator[](const Key &key) {
        std::shared_ptr<T> result;

        auto it = data.find(key);
        if (it != data.end() && it->first == key) {
            result = it->second.lock();
        }
        if (!result) { // May happen even if weak_ptr was found!
            result = std::make_shared<T>(f());
            if (it != data.end() && it->first == key) {
                data.erase(it);
            }
            data.emplace(key, result);
        }
        return result;
    }
    // END SOLUTION

private:
    Factory f;
    // START SOLUTION
    std::unordered_map<Key, std::weak_ptr<T>> data;
    // END SOLUTION
};

int main() {
    auto f = [val = 0]() mutable { return ++val; };
    CachingFactory<std::string, int, decltype(f)> factory(f);
    std::cout << *factory["foo"] << "\n"; // 1
    std::cout << *factory["bar"] << "\n"; // 2
    std::cout << *factory["foo"] << "\n"; // 3

    {
        auto x = factory["foo"]; // 4
        std::cout << *x << "\n";
        std::cout << *factory["foo"] << "\n"; // 4
        std::cout << *factory["bar"] << "\n"; // 5
        std::cout << *factory["foo"] << "\n"; // 4
    }
    std::cout << *factory["foo"] << "\n"; // 6
}
