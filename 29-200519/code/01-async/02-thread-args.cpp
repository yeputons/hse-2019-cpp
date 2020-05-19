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

    template<typename Fn, typename Arg>
    thread(Fn fn, Arg arg0) : thread(
            [/*&*/ /*=*/ fn = std::move(fn), arg0 = std::move(arg0)]() mutable {
                fn(arg0);
            }
    ) {
    }

    ~thread() {
        int res = pthread_join(thread_id, nullptr);
        assert(res == 0);
    }
};

int main() {
    thread t1([]() {
        std::cout << "Hello\n";
    });

    thread t2([](std::string message) {
        std::cout << "message=" << message << "\n";
    }, std::string("something-something"));

#if 0
    std::string msg = "begin>";
    thread([](std::string &msg) {
        msg += "from-thread";
    }, msg);
    msg += "<end";
    std::cout << msg << "\n";
#endif
    return 0;
}
