#include <ecs/Config.hpp>
#include <ecs/entity/Entity.hpp>
#include <ecs/entity/EntityManager.hpp>

#include <iostream>

struct X { X(int x) : x(x) {} ~X() {} int x; };

bool operator == (X const& a, X const& b) {
    return a.x == b.x;
}
bool operator < (X const& a, X const& b) {
    return a.x < b.x;
}

namespace std {
    template <>
    struct hash<X> {
        ::std::size_t operator()(const X& k) const noexcept {
            using ::std::size_t;
            using ::std::hash;
            using ::std::string;

            return hash<int>()(k.x);
        }
    };
}


int main (int , char** ) {
    using namespace ECS_NS_ECS;
    return 0;
}