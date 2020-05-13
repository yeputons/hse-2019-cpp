int main() {
    static_assert(2 * 2 == 4);
    static_assert(2 * 2 == 4, "Ok");
    static_assert(2 * 2 == 5, "Oops");
}
