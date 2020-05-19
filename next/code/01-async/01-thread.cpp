#include <pthread.h>

class thread {
    pthread_t thread_id;
    static void* worker(void *arg) {
        thread *t = static_cast<thread*>(arg);
    }
public:
    template<typename Fn>
    thread(Fn fn) {
        int retval = pthread_create(&thread_id, nullptr, worker, this);
        assert(retval == 0);
    }
    ~thread() {
        pthread_join(thread_id, nullptr);
    }
};

int main() {
}
