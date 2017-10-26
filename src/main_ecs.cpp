#include <ecs/Config.hpp>
#include <ecs/controller/Controller.hpp>
#include <iostream>

struct X { X(int x = -10) : x(x) {} ~X() {} int x; };

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

struct Context {
    using entity_mask = ecs::entity_mask<24, 8>;
    using pool_grow_policy = ecs::instant_grow_policy;
    static constexpr ecs::i32 pool_size = 1024;
    using Time_t = float;
};

int main (int , char** ) {
    using namespace ECS_NS_ECS;

    using C = Controller<Components_list<X>, Systems_list<>, Context>;

    C c;

    auto e = c.create();
    std::cout << e << std::endl;
    std::cout << c.has_component<X>(e) << std::endl;
    c.add<X>(e, 42);
    std::cout << c.has_component<X>(e) << std::endl;
    std::cout << c.get<X>(e).x << std::endl;
    auto e1 = c.create();
    c.add<X>(e1);
    std::cout << c.get<X>(e1).x << std::endl;
    c.get<X>(e).x = 100;
    std::cout << c.get<X>(e).x << std::endl;
    c.reset_components(e);
    //std::cout << c.get<X>(e).x << std::endl;
    std::cout << c.get<X>(e1).x << std::endl;
    
    return 0;
}