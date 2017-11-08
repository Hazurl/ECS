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
};
/*
using Entities_t = int[100];
*/
template<typename...Args>
struct View {};

template<typename E>
struct Mover {
    Mover(int) { std::cout << "Mover Construction" << std::endl; } 
    Mover(Mover const&) { std::cout << "Mover Copy" << std::endl; } 
    ~Mover() { std::cout << "Mover Destruction" << std::endl; }

    void update(ecs::Views<E, X> const& views, int) {
        std::cout << "update 0 " << std::endl;
        for (auto const& view : views) {
            std::cout << "Ent " << view.entity() << " : " << view.template get<X>().x << std::endl;
        }
    }
    void _update(float t, int i) {
        std::cout << "update 1 " << t << " " << i << std::endl;
    }
    void _update(float t) {
        std::cout << "update 2 " << t << std::endl;
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

    using Ent_t = Entity<Entities_::mask>;
    using MoverSystem = System< Mover<Ent_t>, SystemMethods_list< SYST_METH(Mover<Ent_t>::update) > >;
    using Ctx = Context<Components_list<X>, Systems_list< MoverSystem >, Pool_, Entities_>; 

    using C = Controller<Ctx>;

    C c(SystemsConstructor<Systems_list<Mover<Ent_t>>>(
        std::function<Mover<Ent_t>()>([] () { 
            return Mover<Ent_t> { 42 }; 
        }
    )));

    c.add<X>(c.create());
    auto e = c.create();
    c.add<X>(e, -1);
    c.destroy(e);
    c.add<X>(c.create(), 42);

    std::cout << "is views: " << is_views_v<Views<int, X>> << std::endl;
    std::cout << "is views: " << is_views_v<const Views<int, X>> << std::endl;
    std::cout << "is views: " << is_views_v<Views<int, X>&> << std::endl;
    std::cout << "is views: " << is_views_v<const Views<int, X>&> << std::endl;
    std::cout << "is views: " << is_views_v<int> << std::endl;
    std::cout << "is views: " << is_views_v<const int> << std::endl;
    std::cout << "is views: " << is_views_v<int&> << std::endl;
    std::cout << "is views: " << is_views_v<const int&> << std::endl;

    std::cout << "is views: " << is_views_v<mtp::remove_qualifiers<Views<int, X>>> << std::endl;
    std::cout << "is views: " << is_views_v<mtp::remove_qualifiers<const Views<int, X>>> << std::endl;
    std::cout << "is views: " << is_views_v<mtp::remove_qualifiers<Views<int, X>&>> << std::endl;
    std::cout << "is views: " << is_views_v<mtp::remove_qualifiers<const Views<int, X>&>> << std::endl;
    std::cout << "is views: " << is_views_v<mtp::remove_qualifiers<int>> << std::endl;
    std::cout << "is views: " << is_views_v<mtp::remove_qualifiers<const int>> << std::endl;
    std::cout << "is views: " << is_views_v<mtp::remove_qualifiers<int&>> << std::endl;
    std::cout << "is views: " << is_views_v<mtp::remove_qualifiers<const int&>> << std::endl;

    c.update(0);

    return 0;
}