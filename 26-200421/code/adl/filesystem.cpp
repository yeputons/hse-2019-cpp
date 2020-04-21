#include <filesystem>

int main() {
    std::filesystem::path from("a.txt");
    std::filesystem::path to("a-copy.txt");
    copy_file(from, to);
    std::filesystem::copy_file("a.txt", "a-copy-2.txt");
}
