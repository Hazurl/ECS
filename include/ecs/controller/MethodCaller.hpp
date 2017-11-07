#pragma once 

#include <ecs/Config.hpp>
#include <ecs/view/Views.hpp>

#include <utility>
#include <tuple>

ECS_BEGIN_NS

template<typename Entity_t, typename Time_t, typename ViewHelper>
class MethodCallerHelper {
public:

    MethodCallerHelper(Time_t time, ViewHelper* viewHelper) : time(time), viewHelper(viewHelper) {}

    Time_t time;
    ViewHelper* viewHelper;

};

template<typename H, typename T>
class GetArg;

template<typename Entity_t, typename Time_t, typename ViewHelper>
class GetArg<MethodCallerHelper<Entity_t, Time_t, ViewHelper>, Time_t> {
public:
    
    static Time_t get(MethodCallerHelper<Entity_t, Time_t, ViewHelper> m) {
        return m.time;
    }
};

template<typename Entity_t, typename Time_t, typename ViewHelper, typename...ArgsInView>
class GetArg<MethodCallerHelper<Entity_t, Time_t, ViewHelper>, Views<Entity_t, ArgsInView...>> {
public:
    
    static Views<Entity_t, ArgsInView...> get(MethodCallerHelper<Entity_t, Time_t, ViewHelper> m) {
        return m.viewHelper->template construct_views<ArgsInView...>();
    }
};

template<typename T, typename M>
class MethodCaller;

template<typename T, typename R, typename...Args>
class MethodCaller<T, R(T::*)(Args...)> {
public:

    using Method = R(T::*)(Args...);

    template<typename H>
    static R call(T& t, Method m, H args_helper) {
        return (t.*m)(GetArg<H, std::remove_const_t<std::remove_reference_t<Args>>>::get(args_helper)...);
    }

};

ECS_END_NS