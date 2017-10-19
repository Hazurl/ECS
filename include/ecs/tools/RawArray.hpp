#pragma once

#include <ecs/Config.hpp>

ECS_BEGIN_NS

template<typename T>
using RawData = alignas(alignof(T)) i8[sizeof(T)];

template<typename T, ui32 N>
using RawArray = RawData<T>[N];

template<typename T>
T& raw_cast(RawData<T>& r) {
    return reinterpret_cast<T&>(r);
}

template<typename T>
const T& raw_cast(RawData<T> const& r) {
    return reinterpret_cast<const T&>(r);
}

ECS_END_NS