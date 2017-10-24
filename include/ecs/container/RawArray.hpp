#pragma once

#include <ecs/Config.hpp>
#include <utility>

ECS_BEGIN_NS

namespace {

    template<ui32 a, ui32 b>
    static constexpr ui32 Max = a > b ? a : b;

    template<ui32 a, ui32 b>
    static constexpr ui32 Min = a < b ? a : b;
    
}

template<typename T>
struct alignas(T) RawData {
    using type = alignas(T) i8[sizeof(T)];
    type value;

    RawData() = default;
    template<typename...Args>
    RawData(Args&&...args) {
        this->cast() = T(std::forward<Args>(args)...);
    }

    RawData(RawData<T> const&) = delete;
    RawData(RawData<T> && o) {
        copy_raw(o);
    }

    RawData<T>& operator = (RawData<T> const& o) = delete;
    RawData<T>& operator = (RawData<T>&& o) {
        copy_raw(o);
        return *this;
    }

    void copy_raw(RawData<T> const& from) {
        for (ui32 i = 0; i < sizeof(T); ++i)
            value[i] = from.value[i];
    }

    void copy_content(RawData<T> const& from) {
        cast() = from.cast();
    }
    
    T& cast() { 
        return reinterpret_cast<T&>(value); 
    }
    const T& cast() const { 
        return reinterpret_cast<const T&>(value); 
    }
};

template<typename T>
class RawArray {
public:
    using Data_t = RawData<T>;

    RawArray(RawArray<T> const& o) = delete;
    RawArray<T>& operator = (RawArray<T> const& o) = delete;
    
    RawArray(ui32 size) : raw( new Data_t[size] ) {}
    ~RawArray() { delete [] raw; }
    RawArray(RawArray<T>&& o) {
        raw = o.raw;
        o.raw = nullptr;
    }
    RawArray<T>& operator = (RawArray<T>&& o) {
        delete [] raw;
        raw = o.raw;
        o.raw = nullptr;
        return *this;
    }

    void copy_raw(RawArray<T> const& from, ui32 size) {
        for (ui32 i = 0; i < size; ++i)
            (*this)[i].copy_raw(from[i]);
    }

    void copy_content(RawArray<T> const& from, ui32 size) {
        for (ui32 i = 0; i < size; ++i)
            (*this)[i].copy_content(from[i]);
    }

    Data_t& operator [] (ui32 idx) {
        return raw[idx];
    }

    const Data_t& operator [] (ui32 idx) const {
        return raw[idx];
    }

    operator Data_t* () {
        return raw;
    }

    operator const Data_t* () const {
        return raw;
    }

private:

    Data_t* raw = nullptr;    
};

template<typename T, ui32 N>
class SizedRawArray {
public:
    using Data_t = RawData<T>;

    SizedRawArray(SizedRawArray<T, N> const&) = delete;

    SizedRawArray() = default;
    ~SizedRawArray() = default;
    SizedRawArray(SizedRawArray<T, N>&&) = default;
    SizedRawArray<T, N>& operator = (SizedRawArray<T, N>&&) = default;
    
    template<ui32 M = N>
    void copy_raw(SizedRawArray<T, N> const& from, ui32 size = Min<M, N>) {
        for (ui32 i = 0; i < size; ++i)
            (*this)[i].copy_raw(from[i]);
    }

    template<ui32 M = N>
    void copy_content(SizedRawArray<T, M> const& from, ui32 size = Min<M, N>) {
        for (ui32 i = 0; i < size; ++i)
            (*this)[i].copy_content(from[i]);
    }

    Data_t& operator [] (ui32 idx) {
        return raw[idx];
    }

    const Data_t& operator [] (ui32 idx) const {
        return raw[idx];
    }

    operator Data_t* () {
        return raw;
    }

    operator const Data_t* () const {
        return raw;
    }

private:

    Data_t raw[N];    
};

ECS_END_NS