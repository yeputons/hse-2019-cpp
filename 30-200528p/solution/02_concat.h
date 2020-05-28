#ifndef CONCAT_H
#define CONCAT_H

#include <sstream>
#include <string>

namespace cls_30 {
template<typename ...Args>
std::string concat(const Args &...args) {
    std::stringstream s;
    (s << ... << args);  // Fold expression for `operator<<` (C++17)
    // ((s << args), ...);  // Fold expression for `operator,` (C++17)
    // int dummy[] = { (s << args, 0)... };  // Pack expansion + `operator,` (C++11)
    return s.str();
}
}  // namespace cls_30

#endif  // CONCAT_H
