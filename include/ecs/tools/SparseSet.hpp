#pragma once

#include <ecs/Config.hpp>

#include <array>
#include <cassert>

ECS_BEGIN_NS

template<typename T, ui32 Size>
class SparseSet {
public:

    SparseSet()
        :   set(-1)
        ,   size(0) {}

    ~SparseSet() {}

    T* operator [] (ui32 position) {
        if (set[position] == -1)
            return nullptr;
        return &data[set[position]];
    }

    bool has(ui32 position) {
        return (*this)[position] != nullptr;
    }

    T& add(T const& t, ui32 position) {
        auto in_place = find(position);
        if (in_place != nullptr) 
            return *in_place;
        return data[set[position] = size++] = t;
    }

    void erase(ui32 position) {
        if (set[position] != -1) {
            data
        }
    }

private:

    std::array<T, Size> data;
    std::array<ui32, Size> set;

    ui32 size;

};

ECS_END_NS