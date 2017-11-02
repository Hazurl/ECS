#pragma once

#include <ecs/Config.hpp>
#include <ecs/controller/context/Systems.hpp>

#include <mtp/list/List.hpp>

ECS_BEGIN_NS

template<typename...Args>
using Components_list = mtp::List<Args...>;

template<typename...Args>
using Systems_list = mtp::List<Args...>;

template<typename C, typename S, typename Time, typename Pools, typename Entities>
struct Context {
    constexpr Context() = default;
    ~Context() = default;
    static_assert(mtp::AlwaysFalse<C>::value, "To use the context, give it a Component_list and a System_list");
};

template<typename...Crgs, typename...Srgs, typename Time, typename Pools, typename Entities>
struct Context <Components_list<Crgs...>, Systems_list<Srgs...>, Time, Pools, Entities> {
    using Time_t = Time;
/*
    static constexpr i32 pools_size = Pools::size;
    using pools_grow_policy = typename Pools::grow_policy;
    static_assert(pools_size > 0, "Pools's size can't be null or negative");
    static_assert(is_grow_policy<pools_grow_policy>::value, "Pools::grow_policy is unknown");
    
    using entities_mask = typename Entities::mask; 
    static_assert(is_entity_mask_policy<entities_mask>::value, "Entities::mask is unknown");
*/
    using components = Components_list<Crgs...>;

    using systems = mtp::filter<mtp::transform< Systems_list<Srgs...>, TransformSystem>, has_methods>;
    using systems_types = mtp::transform<systems, mtp::type_of>;

    static_assert(mtp::unique_v<systems_types>, "Systems must be unique");
    
};

ECS_END_NS