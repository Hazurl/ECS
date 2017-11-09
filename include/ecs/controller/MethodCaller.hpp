#pragma once 

#include <ecs/Config.hpp>
#include <ecs/view/Views.hpp>

#include <utility>
#include <tuple>

ECS_BEGIN_NS
template<typename T, typename M>
class MethodCaller;

template<typename T, typename R, typename...Args>
class MethodCaller<T, R(T::*)(Args...)> {
public:

    using Method = R(T::*)(Args...);

    template<typename H>
    static void call(T& t, Method m, H & args_helper) {
        return (t.*m)((args_helper.template get<mtp::remove_qualifiers<Args>>())...);
    }

};

ECS_END_NS