#include <cassert>
#include <set>
#include <typeindex>
#include <typeinfo>

using std::set;

int main() {
    //set<const std::type_info&> known_types;
    set<std::type_index> known_types;
    known_types.insert(typeid(int));
    known_types.insert(typeid(short));
    known_types.insert(typeid(std::type_index));
    assert(known_types.size() == 3);
}
