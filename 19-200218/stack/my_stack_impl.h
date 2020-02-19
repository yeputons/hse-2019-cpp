template<typename T>
my_stack<T>::my_stack() noexcept : data(nullptr), len(0), cap(0) {
}

template<typename T>
bool my_stack<T>::empty() const noexcept {
    return !len;
}

template<typename T>
std::size_t my_stack<T>::length() const noexcept {
    return len;
}
