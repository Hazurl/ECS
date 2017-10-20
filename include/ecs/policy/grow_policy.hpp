#pragma once

#include <ecs/Config.hpp>

#include <memory>

ECS_BEGIN_NS

struct tag_grow_policy {};

struct instant_grow_policy {
    using tag = tag_grow_policy;
};

template<i32 N, typename = std::enable_if_t<(N > 0 && (N % 2) == 0)>>
struct bucket_grow_policy {
    using tag = tag_grow_policy;    
};

template<i32 InitialSize, i32 Factor = 2, typename = std::enable_if_t<(Factor > 1 && InitialSize > 0)>>
struct growing_grow_policy {
    using tag = tag_grow_policy;    
};

ECS_END_NS