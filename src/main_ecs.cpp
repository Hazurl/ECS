#include <ecs/Config.hpp>
#include <ecs/component/Component.hpp>
#include <ecs/component/ComponentPool.hpp>
#include <ecs/tools/BiMap.hpp>

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

    ComponentPool<X, 2048, growing_grow_policy<256>> pool;
    //ComponentPool<X, 2048, bucket_grow_policy<256>> pool;
    //ComponentPool<X, 2048, instant_grow_policy> pool;
    pool.add(10, 10);
    pool.remove(10);
    pool.add(10, 10);
    for(int i = 0; i < 32; i++) pool.add(1024 - i, i * 2);
    pool.remove(1000);
    pool.for_each([] (auto i) {
        std::cout << i.x << std::endl;
    });

    return 0;
}