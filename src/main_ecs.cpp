#include <ecs/Config.hpp>
#include <ecs/entity/Entity.hpp>

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

    using E = Entity<entity_4_12_mask>;

    E e0 = E::create(0u, 0u);
    std::cout << "E0 <" << (i64)e0.id() << ":" << (i64)e0.version() << ">" << std::endl; 
    E e1 = E::create(0u, 1u);
    std::cout << "E1 <" << (i64)e1.id() << ":" << (i64)e1.version() << ">" << std::endl; 
    E e2 = E::create(1u, 0u);
    std::cout << "E2 <" << (i64)e2.id() << ":" << (i64)e2.version() << ">" << std::endl; 
    E e3 = E::create(128u, 0u);
    std::cout << "E3 <" << (i64)e3.id() << ":" << (i64)e3.version() << ">" << std::endl; 
    E e4 = E::create(56u, 3u);
    std::cout << "E4 <" << (i64)e4.id() << ":" << (i64)e4.version() << ">" << std::endl; 
    E e5 = E::create(1000u, 1000u);
    std::cout << "E5 <" << (i64)e5.id() << ":" << (i64)e5.version() << ">" << std::endl; 
    
    return 0;
}