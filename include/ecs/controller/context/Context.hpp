#pragma once

#include <ecs/Config.hpp>
#include <ecs/controller/context/Systems.hpp>
#include <ecs/container/Tuple.hpp>

#include <mtp/list/List.hpp>

#include <functional>
#include <tuple>

ECS_BEGIN_NS

template<typename...Args>
using Components_list = mtp::List<Args...>;

template<typename...Args>
using Systems_list = mtp::List<Args...>;

template<typename C, typename S, typename Time, typename Pools, typename Entities>
struct Context {
    static_assert(mtp::AlwaysFalse<C>::value, "To use the context, give it a Components_list and a Systems_list");
};

template<typename...Crgs, typename...Srgs, typename Time, typename Pools, typename Entities>
struct Context <Components_list<Crgs...>, Systems_list<Srgs...>, Time, Pools, Entities> {
    // TIME
        // type use as time, given when a system is updated
    using Time_t = Time;

    // POOLS
        // size of each pool
    static constexpr i32 pools_size = Pools::size;
        // policy use in container to grow
    using pools_grow_policy = typename Pools::grow_policy;
    static_assert(pools_size > 0, "Pools's size can't be null or negative");
    static_assert(is_grow_policy<pools_grow_policy>::value, "Pools::grow_policy is unknown");
    
    // ENTITIES
        // Mask for each entity
    using entities_mask = typename Entities::mask; 
    static_assert(is_entity_mask_policy<entities_mask>::value, "Entities::mask is unknown");

    // COMPONENTS
        // List of components ( type + meta )
    using components = Components_list<Crgs...>;
        // List of components type
    using components_type = components;
    static_assert(mtp::unique_v<components_type>, "Components must be unique");
    
    // SYSTEMS
        // List of System ( type + meta )
    using systems = mtp::transform< Systems_list<Srgs...>, TransformSystem>;
        // List of systems type
    using systems_type = mtp::transform<systems, mtp::type_of>;
    static_assert(mtp::unique_v<systems_type>, "Systems must be unique");
};

template<typename S>
class SystemsConstructor {
    static_assert(mtp::AlwaysFalse<S>::value, "You must use a Systems_list");
};

template<typename...Srgs>
class SystemsConstructor<Systems_list<Srgs...>> : std::tuple<std::function<Srgs()>...> {
    template<typename S> using F = std::function<S()>;
    static_assert(mtp::unique_v<Systems_list<Srgs...>>, "Systems must be unique");
    
public:

    template<typename S>
    static constexpr bool has_constructor_for = mtp::count_v<Systems_list<Srgs...>, S> > 0;

    SystemsConstructor(std::function<Srgs()>...funcs) {
        assign<Srgs...>(funcs...);
    }

    template<typename S>
    S construct_system() {
        return std::get<F<S>>(*this)();
    }

private:

    template<typename S, typename...Ss>
    void assign(F<S> func, F<Ss>...funcs) {
        assign<S>(func);
        assign<Ss...>(funcs...);
    }

    template<typename S>
    void assign(F<S> func) {
        std::get<F<S>>(*this) = func;
    }
};

ECS_END_NS