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

    static constexpr i64 id_mask        = ~((~0LL) << id_bits_size);
    static constexpr i64 version_mask   = ~((~0LL) << version_bits_size);
};

ECS_END_NS