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
    Mover() { std::cout << "M Construction" << std::endl; } 
    ~Mover() { std::cout << "M Destruction" << std::endl; }
    void update(MyContext::Time_t) {
        std::cout << "update 0" << std::endl;
    }
    /*void update(MyContext::Time_t, int) {
        std::cout << "update 1" << std::endl;
    }*/
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
int main (int , char** ) {
    using namespace ECS_NS_ECS;

    using C = Controller<Components_list<X>, Systems_list<Mover>, MyContext>;

    using SystMover = System<Mover, SystemMethods_list< SYST_METH(Mover::update) >>;
    using context = Context<Components_list<>, Systems_list< Mover, System< X, SystemMethods_list<>> >, float, void, void>;

    using _ = mtp::ShowType<context::systems>;
    using __ = mtp::ShowType<context::systems_types>;

    return 0;
}

