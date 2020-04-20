## If with init statement (C++17)
<!-- На лекции, чтобы попало на экзамен. -->

До:
```c++
map<int, string> m = ....;
{
    typename map<int, string>::iterator it = m.lower_bound(10);
    if (it != m.end()) cout << it->second << '\n';
    else               cout << "Not found\n";
} // После этой скобки `it` недоступен
```

После (строго лучше): <!-- обвести два куска, проверсти аналогию с `for` -->
```c++
map<int, string> m = ....;
if (auto it = m.find(10); it != s.end()) {
    cout << it->second << '\n';
} else {
    cout << "Not found\n";  // it тут доступен.
} // После этой скобки `it` недоступен.
```

Аналогично полезно с `.find()` и `dynamic_cast<T*>`, смотри [`25-200416p/problems/01-basic/06-dynamic-cast.cpp`](https://github.com/yeputons/hse-2019-cpp/blob/26daebb971d2c5b6f8f995e7aa40fdf9ebfce486/25-200416p/problems/01-basic/06-dynamic-cast.cpp)

---
## Передача параметров в функции и умные указатели
Смотри [GotW 91](https://herbsutter.com/2013/06/05/gotw-91-solution-smart-pointer-parameters/) (Guru of the Week, автор — Herb Sutter).

* Практически всегда по значению, `&&` и умные указатели не нужны:
    ```c++
    void foo(vector<int> foo_data) { sort(foo_data.begin(), foo_data.end(); .. };
    int main() {
        vector<int> data = {1, 2, 3, 4, 5};
        foo(data);  // Тут скопируем, вектор ещё нужен.
        data.emplace_back(6);  // {1, 2, 3, 4, 5, 6}
        foo(std::move(data));  // Разрешили не копировать.
        data.clear();  // Вектор мог остаться непустым, не определено moved-from.
    }
    ```

* Если хотим отдать параметр/переменную — всегда пишем `std::move`:
    ```c++
    struct MyVector {
        vector<int> data;
        MyVector(vector<int> data_) : data(std::move(data_)) {
            // Тут состояние data_ не определено.
        }
    };
    ```

---
## Параметры только для чтения/для записи
* Если нам вcегда передают объект, а мы его только читаем:
  ```c++
  void printVector(const vector<int> &data);
  ```

* Если иногда не передают — `std::optional` (не указатель!):
  ```c++
  void maybePrintVector(const std::optional<vector<int>> &data);
  ```

* Если нам всегда нужно передать наружу объект (не указатель!):
  ```c++
  void readAndAppendToVector(vector<int> &data);
  ```
  _Осторожно_: а что, если `data` исходно непуст? Не надо думать — верните по значению.

* Если пользователь не всегда хочет ответ — указатель.
  <!-- Указатель здесь лучше ссылки только потому что в нём может быть `nullptr`. В указатель запишем ответ. Владеет им вызывающий. -->
  ```c++
  void readAndMaybeAppendToVector(vector<int> *const data);
  ```

---
## Умные указатели в параметрах
Умный указатель — пара `(данные, владение)`.
В параметрах — только если нам важно, как именно им владеет __вызвавший__.
Обычно неважно.

* `const unique_ptr<T>&` — лучше заменить на `T&` (`const` явно исчез). <!-- константен только сам указатель; а вдруг у нас вообще объект на стеке? Тоже уникальный владелец -->
  * Это не семантика &laquo;у объекта один владелец&raquo; — стэк, поле...
* `unique_ptr<T>&` — почти никогда <!-- пример: swap, за деталями — в GotW -->
* `const shared_ptr<T>&` — почти никогда <!-- пример: когда иногда хотим себе скопировать, за деталями — в GotW  -->
* `shared_ptr<T>&*` — почти никогда
*   ```c++
    // Окно нельзя копировать, важно считать ссылки и владеть совместно.
    void addToAnotherDesktop(shared_ptr<Window> window) {  // По значению.
        recentlyMovedWindows.emplace_back(window);         // Можем копировать.   
        myWindows.emplace_back(std::move(window));         // Можем мувать.
    }
    ```
*   ```c++
    Node(const Node &left_, const Node &_right)
        : left(make_unique<Node>(left_)), .... {}
    // Оптимизация: всегда оборачиваем в `unique_ptr`, давайте сразу его возьмём.
    Node(unique_ptr<Node> left_, unique_ptr<Node> right_)  // Без &&
        : left(std::move(left_)), right(std::move(right_)) {}
    ```
