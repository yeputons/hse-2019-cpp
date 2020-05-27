#ifndef CONCAT_H
#define CONCAT_H

#include <sstream>
#include <string>

namespace cls_30 {
template<typename ...Args>
std::string concat(const Args &...args) {
    std::stringstream s;
    (s << ... << args);
    return s.str();
}
}  // namespace cls_30

#endif  // CONCAT_H
