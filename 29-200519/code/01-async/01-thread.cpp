#include <cassert>
#include <functional>
#include <iostream>
#include <pthread.h>
#include <utility>

class thread {
private:
    pthread_t thread_id;
    std::function<void()> fn;

    static void *worker(void *arg) {
        thread *t = static_cast<thread *>(arg);
        t->fn();
        return nullptr;
    }

public:
    thread(std::function<void()> fn_) : fn(std::move(fn_)) {
        int res = pthread_create(&thread_id, nullptr, worker, this);
        assert(res == 0);
    }

    ~thread() {
        int res = pthread_join(thread_id, nullptr);
        assert(res == 0);
    }
};

int main() {
    thread t([]() {
        std::cout << "Hello\n";
    });
    std::cout << "World\n";
    return 0;
}
