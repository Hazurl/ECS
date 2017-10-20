#pragma once

#include <ecs/Config.hpp>
#include <utility>

ECS_BEGIN_NS

template<typename T>
using RawData = alignas(alignof(T)) i8[sizeof(T)];

template<typename T, ui32 N>
using RawArray = RawData<T>[N];

template<typename T>
using RawArrayPtr = RawData<T>*;

template<typename T>
RawArrayPtr<T> new_RawArray(ui32 size) {
    return static_cast<RawArrayPtr<T>>( new RawData<T>[size] );
}

template<typename T>
void delete_RawArray(RawArrayPtr<T>& arr) {
    delete [] arr;
}

template<typename T>
void copy_RawArray(RawArrayPtr<T> const& from, RawArrayPtr<T>& to, ui32 size) {
    for (ui32 i = 0; i < size; ++i)
        (*to)[i] = (*from)[i];
}

template<typename T>
void move_RawArray(RawArrayPtr<T>& from, RawArrayPtr<T>& to, ui32 size) {
    for (ui32 i = 0; i < size; ++i)
        (*to)[i] = std::move((*from)[i]);
}

template<typename T>
T& raw_cast(RawData<T>& r) {
    return reinterpret_cast<T&>(r);
}

template<typename T>
const T& raw_cast(RawData<T> const& r) {
    return reinterpret_cast<const T&>(r);
}

ECS_END_NS