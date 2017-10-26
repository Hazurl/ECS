#pragma once

#include <ecs/Config.hpp>
#include <mtp/Utils.hpp>

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

template<typename grow_policy, typename = void>
struct is_grow_policy : mtp::boolConst<false> {};

template<typename grow_policy>
struct is_grow_policy<grow_policy, mtp::Nothing<
        std::enable_if_t<std::is_same<typename grow_policy::tag, tag_grow_policy>::value>
    >> : mtp::boolConst<true> {};

static_assert(is_grow_policy<bucket_grow_policy<64>>::value, "!");
static_assert(is_grow_policy<growing_grow_policy<64>>::value, "!");
static_assert(is_grow_policy<instant_grow_policy>::value, "!");

ECS_END_NS