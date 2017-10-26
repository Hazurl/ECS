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

    C::Entity_t ent = 42;
    X x(1337);
    double i = -1.1;

    C::View<X, double> v(ent, std::make_tuple(&x, &i));

    std::cout << v.get<X>().x << std::endl;
    std::cout << v.get<double>() << std::endl;
    std::cout << v.get<int>() << std::endl;
    
    return 0;
}