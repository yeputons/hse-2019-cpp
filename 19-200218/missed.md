Не на экзамен: на самом деле можно поймать исключение, которое вылетает в `return`.
Просто `try`-блоком. Или function-try-block зачем-то. Вроде это может помочь со `stack::pop()`,
но там дальше проблемы вида `Foo f; f = my_stack.pop();`, хотя тут, строго говоря, виноват `operator=`.

