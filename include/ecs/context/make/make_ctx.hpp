#pragma once

#include <ecs/Config.hpp>

#include <ecs/policy/grow_policy.hpp>
#include <ecs/policy/mask_policy.hpp>

#include <ecs/entity/Entity.hpp>
#include <ecs/entity/EntityController.hpp>

#include <mtp/list/List.hpp>

ECS_BEGIN_NS
ECS_BEGIN_NS_CTX

namespace {

template<typename entity_mask, typename = std::enable_if_t<is_entity_mask_policy<entity_mask>>>
using _make_entity = Entity<entity_mask>;

template<typename...Components, typename = std::enable_if_t<mtp::unique_v<mtp::List<Components...>>>>
using _make_components = mtp::List<Components...>;

template<typename grow_policy, typename = std::enable_if_t<is_grow_policy<grow_policy>>>
using _make_grow_policy = grow_policy;

}

// Entity
template<typename entity_mask>
using make_entity = _make_entity<entity_mask>;
using default_entity = make_entity<entity_32_mask>;

// Components
template<typename...Components>
using make_components = _make_components<Components...>;

// Grow policy
template<typename grow_policy>
using make_grow_policy = _make_grow_policy<grow_policy>;
using default_grow_policy = instant_grow_policy;

// Limit size
template<ui32 limit>
using make_limit_size = mtp::ui32_<limit>;
using default_limit_size = make_limit_size<256>;

// Views
template<typename entity_t, typename...Comps>
using make_views = Views<entity_t, Comps...>;

// Entity Controller
template<typename entity_t, typename Components, typename limit_size = default_limit_size, typename grow_policy = default_grow_policy>
using make_entity_controller = typename EntitiesController<entity_t, Components, limit_size, grow_policy>::user_bridge;

ECS_END_NS_CTX
ECS_END_NS