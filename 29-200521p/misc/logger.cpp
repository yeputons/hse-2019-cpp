#include <iostream>
#include <utility>
#include <string>

template<typename Fn>
class log {  // Function is probably better because it can then log logged function (copy ctor and stuff).
    Fn fn_;
    std::string name_;
    int indent_ = 0;

    class ArgsLogger {
        log& log_;
    public:
        template<typename ...Args>
        ArgsLogger(log *l, const Args &...args) : log_(*l) {
            std::cerr << std::string(2 * log_.indent_, ' ') << log_.name_ << "(";
            ((std::cerr << " " << args), ...);
            std::cerr << ")\n";
            log_.indent_++;
        }

        template<typename T>
        decltype(auto) ret(T &&value) {
            std::cerr << std::string(2 * log_.indent_ - 2, ' ') << "= " << value << "\n";
            return std::forward<T>(value);
        }

        ~ArgsLogger() {
            log_.indent_--;
        }
    };

public:
    log(Fn fn, std::string name) : fn_(std::move(fn)), name_(std::move(name)) {
    }

    template<typename ...Args>
    decltype(auto) operator()(Args &&...args) & {
        ArgsLogger logger(this, args...);
        return logger.ret(fn_(std::forward<Args>(args)...));
    }

    template<typename ...Args>
    decltype(auto) operator()(Args &&...args) && {
        ArgsLogger logger(this, args...);
        return logger.ret(std::move(fn_)(std::forward<Args>(args)...));
    }

    template<typename ...Args>
    decltype(auto) operator()(Args &&...args) const& {
        ArgsLogger logger(this, args...);
        return logger.ret(fn_(std::forward<Args>(args)...));
    }
};

#define LOG(fn) log(fn, #fn)

int fib_(int n);
auto fib = LOG(fib_);

int fib_(int n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}

int main() {
    std::cout << fib(5) << "\n";
}
