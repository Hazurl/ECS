#include <ecs/Config.hpp>
#include <ecs/controller/Controller.hpp>
#include <ecs/controller/context/Context.hpp>
#include <ecs/container/Tuple.hpp>

#include <iostream>

struct X { X(int x = -10) : x(x) { std::cout << "Construction" << std::endl; } ~X() { std::cout << "Destruction" << std::endl; } int x; };

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

struct MyContext {
    using entity_mask = ecs::entity_mask<24, 8>;
    using pool_grow_policy = ecs::instant_grow_policy;
    static constexpr ecs::i32 pool_size = 1024;
    using Time_t = float;
};
/*
using Entities_t = int[100];
*/
template<typename...Args>
struct View {};

struct Mover {
    Mover(int) { std::cout << "Mover Construction" << std::endl; } 
    Mover(Mover const&) { std::cout << "Mover Copy" << std::endl; } 
    ~Mover() { std::cout << "Mover Destruction" << std::endl; }
    void update(MyContext::Time_t) {
        std::cout << "update 0" << std::endl;
    }
    void update(MyContext::Time_t, int) {
        std::cout << "update 1" << std::endl;
    }
    void _update(MyContext::Time_t) {
        std::cout << "update 2" << std::endl;
    }
};
/*
template<typename Context, typename T, typename F>
struct getViewArgs {
    static_assert(sizeof(T) == 0, "!");
    using type = void;
};

template<typename Context, typename T, typename...Args>
struct getViewArgs<Context, T, void(T::*)(typename Context::Time_t, View<Args...>)> {
    using type = mtp::List<Args...>;
};
*/

struct Pool_ { 
    using grow_policy = ecs::instant_grow_policy; 
    static constexpr int size = 256; 
};

struct Entities_ {
    using mask = ecs::entity_32_mask;
};

int main (int , char** ) {
    using namespace ECS_NS_ECS;

    using MoverSystem = System< Mover, SystemMethods_list< Methods< void(Mover::*)(MyContext::Time_t, int), &Mover::update > > >;
    using Ctx = Context<Components_list<X>, Systems_list< MoverSystem >, float, Pool_, Entities_>; 

    using C = Controller<Ctx>;
/*
    C c(SystemsConstructor<Systems_list<Mover>>(
        std::function<Mover()>([] () { 
            return Mover { 42 }; 
        }
    )));

    for (int i = 0; i < 10; i++) {
        c.add<X>(c.create(), i);
    }

    auto v = c.views<X>();
    for (auto x : v) {
        std::cout << x.entity() << "::X.x : " << x.get<X>().x << std::endl;
    }

    c.add<X>(c.create(), 42);
    for (auto x : v) {
        std::cout << x.entity() << "::X.x : " << x.get<X>().x << std::endl;
    }
*/

    SparseSet<X, 16, int> set;

    for (int i = 0; i < 16; i+=2) {
        set.add(i, -i / 2);
    }

    for(auto& x : set) {
        std::cout << x.x << std::endl;
    }

    for(auto& idx : set.keys()) {
        std::cout << "At " << idx << " : " << set.get(idx).x << std::endl;
    }
    
    return 0;
}

