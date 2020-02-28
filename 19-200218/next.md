Зачем `using`, когда это экономит всего ничего символов?

`template`/`typename`: зачем вкладывать шаблоны в шаблоны?

## Сложности с `operator=`:
* Без copy-and-swap:
  ```
  template<typename T> stack<T>& stack::operator=(const stack &other) {
      if (this == &other) return *this;  // Необязательно.
      reserve(other.len);
      for (size_t i = 0; i < other.len; i++)
          data[i] = other.data[i];  // Если кинуло, то упс.
      len = data[i];
      return *this;
  }
  ```
* Правильнее (никогда не портим старые данные до самого конца):
  ```
  template<typename T> stack<T>& stack::operator=(const stack &other) {
      if (this == &other) return *this;  // Обязательно!
      /* unique_ptr<T/*[]*/>, иначе утечка */ T *newdata = new T[other.len];
      for (size_t i = 0; i < other.len; i++) // Можно вынести в функцию, чтобы не дублировать код.
          newdata[i] = other.data[i];
      delete[] data;  // Не должно кидать.
      data = newdata;
      len = cap = other.len;
      return *this;
  }
  ```
