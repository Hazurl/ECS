#pragma once

#include <ecs/Config.hpp>

ECS_BEGIN_NS

struct tag_mask_policy {};

template<ui8 id_bits_size, ui8 version_bits_size>
struct entity_mask {
    using tag = tag_mask_policy;
    
    static constexpr i64 total_size     = (id_bits_size + version_bits_size) / 8 + bool((id_bits_size + version_bits_size) % 8);
    
    static constexpr i64 id_size        = (id_bits_size      ) / 8 + bool((id_bits_size     ) % 8);
    static constexpr i64 version_size   = (version_bits_size ) / 8 + bool((version_bits_size) % 8);

    static constexpr i64 id_shift       = 0;
    static constexpr i64 version_shift  = id_bits_size;

    static constexpr i64 id_mask        = ~((~0LL) << id_bits_size     );
    static constexpr i64 version_mask   = ~((~0LL) << version_bits_size);
};

template<typename entity_mask_policy, typename = void>
struct is_entity_mask_policy : mtp::boolConst<false> {};

template<typename entity_mask_policy>
struct is_entity_mask_policy<entity_mask_policy, mtp::Nothing<
        std::enable_if_t<std::is_same<typename entity_mask_policy::tag, tag_mask_policy>::value>,
        mtp::Constant<i64, entity_mask_policy::total_size>,
        mtp::Constant<i64, entity_mask_policy::id_size>,
        mtp::Constant<i64, entity_mask_policy::version_size>,
        mtp::Constant<i64, entity_mask_policy::id_shift>,
        mtp::Constant<i64, entity_mask_policy::version_shift>,
        mtp::Constant<i64, entity_mask_policy::id_mask>,
        mtp::Constant<i64, entity_mask_policy::version_mask>
    >> : mtp::boolConst<true> {};

using entity_32_mask = entity_mask<24, 8>;

static_assert(is_entity_mask_policy<entity_32_mask>::value, "!");

ECS_END_NS