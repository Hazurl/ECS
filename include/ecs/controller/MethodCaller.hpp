#pragma once 

#include <ecs/Config.hpp>

#include <utility>

ECS_BEGIN_NS

class MethodCallerHelper {
public:
    template<typename T>
    T get () = delete;
};

#define MethodCallerHelper_SPE(T) template<> T MethodCallerHelper::get<T>()

MethodCallerHelper_SPE(float) {
    return 42.0f;
}
MethodCallerHelper_SPE(int) {
    return 1337;
}

template<typename T, typename M>
class MethodCaller;

template<typename T, typename R, typename...Args>
class MethodCaller<T, R(T::*)(Args...)> {
public:

    using Method = R(T::*)(Args...);

    template<typename H>
    static R call(T& t, Method m, H&& args_helper) {
        return (t.*m)(args_helper.template get<std::remove_const_t<std::remove_reference_t<Args>>>()...);
    }

};

ECS_END_NS