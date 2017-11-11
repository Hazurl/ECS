#pragma once

#include <ecs/Config.hpp>

#include <mtp/list/List.hpp>

ECS_BEGIN_NS

#define SYST_METH(x...) Methods<decltype(&x), (&x)>

template<typename F, F f>
struct Methods {
    using type = F;
    static constexpr F function = f;
/*
    template<typename C, typename...Args>
    constexpr static auto call (C&& c, Args&&...args) {
        return (c.*function)(std::forward<Args>(args)...);
    } */
};

template<typename T, typename M>
struct System {
    using type = T;
    using methods = M;
};

template<typename...Args>
using SystemMethods_list = mtp::List<Args...>;

template<typename S>
struct TransformSystemHelper : mtp::TConst<System<S, SystemMethods_list< SYST_METH(S::update) >>> {};

template<typename S, typename M>
struct TransformSystemHelper<System<S, M>> : mtp::TConst<System<S, M>> {};

template<typename S>
using TransformSystem = typename TransformSystemHelper<S>::type;

ECS_END_NS