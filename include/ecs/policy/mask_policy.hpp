#pragma once

#include <ecs/Config.hpp>

ECS_BEGIN_NS

struct tag_mask_policy {};

struct entity_16_48_mask {
    using tag = tag_mask_policy;

    static constexpr i64 total_size     = 8;

    static constexpr i64 id_size        = 6;
    static constexpr i64 version_size   = 2;
    
    static constexpr i64 id_shift       = 0;
    static constexpr i64 version_shift  = 48;

    static constexpr i64 id_mask        = 0x0000FFFFFFFFFFFF;
    static constexpr i64 version_mask   = 0x000000000000FFFF;
};

struct entity_8_24_mask {
    using tag = tag_mask_policy;

    static constexpr i64 total_size     = 4;

    static constexpr i64 id_size        = 3;
    static constexpr i64 version_size   = 1;
    
    static constexpr i64 id_shift       = 0;
    static constexpr i64 version_shift  = 24;

    static constexpr i64 id_mask        = 0x00FFFFFF;
    static constexpr i64 version_mask   = 0x000000FF;
};

struct entity_4_12_mask {
    using tag = tag_mask_policy;

    static constexpr i64 total_size     = 2;
    
    static constexpr i64 id_size        = 2;
    static constexpr i64 version_size   = 1;

    static constexpr i64 id_shift       = 0;
    static constexpr i64 version_shift  = 12;

    static constexpr i64 id_mask        = 0x0FFF;
    static constexpr i64 version_mask   = 0x000F;
};

struct entity_2_6_mask {
    using tag = tag_mask_policy;

    static constexpr i64 total_size     = 1;
    
    static constexpr i64 id_size        = 1;
    static constexpr i64 version_size   = 1;

    static constexpr i64 id_shift       = 0;
    static constexpr i64 version_shift  = 6;

    static constexpr i64 id_mask        = 0b00111111;
    static constexpr i64 version_mask   = 0b00000011;
};

ECS_END_NS