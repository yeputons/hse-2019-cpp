# Долги [00:05]
* Не надо писать `try { /* ... */ } catch (...) { throw; }`.
* Тонкость: строгих гарантий `erase`, `insert`, `emplace` для `vector`/`deque` дают не всегда.
  Но стараются. Надо читать для точности.
  Для ассоциативных контейнеров тоже не факт.
* Можно инициализировать через brace-init (как массивы), даже pair и map:
  `vector<vector<int>> = {{1, 2, 3}, {4, 5}};`
* `advance` — это `+=`, а для `+` есть `std::next(it, cnt)`

# Алгоритмы [00:35]
## Введение [00:05]
https://en.cppreference.com/w/cpp/header/algorithm

Всегда либо `==` (отношение эквивалентности: рефлексивность, симметричность, транзитивность),
либо `<` (отношение линейного порядка: антирефлексивность; если ни один не меньше, то равны; транзитивность).
Активно используются функторы, всегда можно выдать свой вместо
стандартного (`std::less`).

Никогда не управляют временем жизни объектов. Это к контейнерам.
Только переприсваивают.

Если что-то не нашёл, то обычно возвращается `end()`/`last`.

## Немодифицирующие [00:04]
* `bool all_of(first, last, pred)` и друзья
* `Pred for_each(first, last, pred)`, `for_each_n` (только первые `n` элементов)
* `difference_type count(first, last, value)`, `count_if(...., pred)`
* `find(first, last, pred)`, `find_if`, `find_if_not`

## Сравнения [00:04]
* `min`/`max`/`minmax` из двух значений или из фигурных скобок. Осторожно: возвращают ссылку:
  https://www.youtube.com/watch?v=s9vBk5CxFyY
  ```
  const Foo &x = foo();  // Работает.
  const Foo &x = std::min(foo1(), foo2());  // Не работает, потому что lifetime extension только с самым внешним значением.
  ```
* `clamp(value, lo, hi)` — обрезаем значение. Возвращает ссылку.
* `equal(f1, l1, f2[, pred])`, ещё есть лексикографическое `<`
* `std::pair<> mismatch(first1, last1, first2)`, первое место с отличием
* `min_element`, `max_element`, `minmax_element` — первый минимум/максимум. Возвращают итератор.
  Пример, когда вектор непустой: `vec.erase(min_element(vec.begin(), vec.end()));` 

Из numeric library:

* `accumulate(first, last, init = 0)` для `operator+`

## Модифицирующие [00:05]
Осторожно с пересечениями входа и выхода!

* `copy(f, l, out)`/`move`{,`_backward`}
  Если куда-то копия, то конечный итератор не даём, он выводится.
  Этот и дальше стараются работать даже для `ForwardIt`, по этому поводу могут активно возвращать указатель на "конец".
* `fill(f, l, value)`, `fill_n(first, count, value)`
* `generate(f, l, g)`, `OutputIt generate_n(first, count, gen)`, есть частные случаи в Numerics library вроде 
* `OutIt transform(f, l, out, unary_op)` как map из Haskell
* `sort` (необязательно стабильный, гарантированный NlogN)/`stable_sort`/`nth_element`,
  есть частичные сортировки...

Из numeric library:

* `partial_sum`, `inclusive_scan`, `exclusive_scan`

## Сужающие [00:07]
* `{remove}{,_copy}{,_if}`: `(first, last, [out], value|pred)`, возвращает итератор на конец.
* `{replace}{,_copy}{,_if}`: `(first, last, [out], old_value|pred, new_value)`, возвращает итератор на конец.
* `unique{,_copy}(f, l, [pred])`, удаляет __подряд идущие__ одинаковые.

Пример:
```
vector<int> v = { 1, 3, 2, 3 };
v.erase(remove_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; }), v.end());
v.erase(unique(v.begin(), v.end()), v.end());
```

## Binary search [00:05]
* `bool binary_search(f, l, [pred])`
* `pair<> equal_range(f, l, val, [pred])`
* `lower_bound` и `upper_bound` как крайние случаи.

## Прочие операции [00:05]
* `swap(a, b)` для значений
* `swap_ranges(f1, l1, f2) --> l2`
* `iter_swap(a, b)` для обмена значений по итераторам
* `reverse{_,copy}` — угадайте, какие параметры?
* `rotate{_,copy}` (сделай элемент первым)
* `partition(f, l, pred) --> mid` за линию, есть модификации и проверка `is_partitioned`
* Merge отсортированных с дополнительной памятью и без
* Операции с отсортированными множествами в векторах: объединение, симметрическая разность, проверка принадлежности...
* Операции с кучей на массиве
* Операции с перестановками на массиве (проверить, получить следующую)

# Свои алгоритмы [00:15]
## `value_type` [00:05]
* Хотим написать `min` по range:
  ```
  template<typename It>
  auto min_element(It first, It last) {
      auto found = *first;
      for (++first; first != last; ++last) {
          if (*first < found) {
              found = *first;
          }
      }
      return found
  }
  ```
* До C++11 нельзя было написать `auto`.
  Поэтому у итераторов и контейнеров есть `value_type`:
  ```
  typename It::value_type min_element(....) { .... }
  ```

## Tag dispatching [00:10]
* Хотим сделать свой `std::next`:
  ```
  template<typename It>
  It my_next(/*std::random_access_iterator_tag, */It x, typename It::difference_type count) {
      return x += count;
  }
  ```
* Тут работает только для RandomAccess. Для ForwardIterator:
  ```
  template<typename It>
  It my_next(/*std::forward_iterator_tag, */It x, typename It::difference_type count) {
      for (; count; --count)
          ++x;
      return x;
  }
  ```
* А теперь фокус (tag dispatching): есть специальный тип `It::iterator_category`.
  Там бывает `std::random_access_iterator_tag` (пустая структура), наследуется
  от `std::bidirectional_iterator_tag`, наследуется от `std::forward_iterator_tag`.
  Так можно надо на этапе компиляции поставить `if`.
  Можно добавить фиктивный параметр и сделать перегрузки,
  а общую реализацию такую:
  ```
  template<typename It>
  It my_next(It x, typename It::difference_type count) {
      return my_next(typename It::iterator_category(), x, count);
  }
  ```
* Это не сработает с `T*`, хотя они тоже указатели.
  Поэтому `std::iterator_traits<It>::iterator_category`.
  Костыль вроде `std::begin`/`std::end`

# Последовательные контейнеры [00:15]
## list [00:07]
* Двусвязный список, удаление всегда за линию от количества элементов.
* Нельзя random access, итераторы и указатели не инвалидируются.
  Можно идти с начала, можно идти с конца за константу.
* Можно перемещать элементы из одного списка в другой:
  * `splice(pos, x)` (если `x != this`): утащили все элементы из `x` (это контейнер или итератор из одного элемента) в позицию `pos`.
    Итераторы и ссылки тоже переехали.
    Всё за константу.
  * `splice(pos, begin, end)`: перетащили внутри себя элементы за константу на другое место.
    А если это был другой список, то линия (потому что надо пересчитать `size`; до C++11 было по-другому).
* Есть `remove`, `remove_if`, `unique`, `merge`, `reverse`, стабильный `sort`: аналогично алгоритмам, но не инвалидируют.

## forward_list [00:03]
* Односвязный список, детально не разбираем.
* Куча проблем с тем, что нельзя просто взять и сделать `insert`, надо `insert_after`.
  Свои методы в больших количествах.

## string [00:05]
TODO

rope, не гарантируется последовательная память
c_str() и data()
substr (индексы)
find (индексы)
append, +, +=
basic_string
wstring == `basic_string<wchar_t>`
можно даже traits для сравнений символов

/////

# Адаптеры контейнеров
stack, queue, priority_queue
bitset

# Ассоциативные контейнеры
* переупорядочивают элементы для быстрого поиска O(logN)
* возможные реализации: дерево поиска O(logN)
* set, map, multiset, multimap; отличия
* Требуют отношение порядка: для элементов должен быть определен \texttt{operator<(...)}
* Нет произвольного доступа по индексу

Требуют порядка на элементах

Методы по сравнению с обычными контейнерами: сравнение, erase по key, insert с подсказкой, count, find

TODO: Что возвращает insert/emplace

Нельзя менять добавленные элементы (`value_type = const Key` для `set`).

`operator[]` всегда создаёт элемент
свой функтор-компаратор, без состояния и с состоянием. Осторожно с трёмя свойствами (антирефлексивность, например).
value_type = `pair<const Key, Value>`
range-based for со structural binding.

внутренний метод `lower_bound`/`equal_range` (внешний можно или нельзя?)
`count` у `multiset`/`multimap` работает за линию.

`emplace` вместо `insert(make_pair(...))`/`insert(pair(...))`

напоминание: как стирать элементы по условию из `map`? Надо не увеличивать итератор удалённого элемента.

## Неупорядоченные ассоциативные контейнеры
для unordered свой хэш
