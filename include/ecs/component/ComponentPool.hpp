#pragma once

#include <ecs/Config.hpp>

#include <array>
#include <unordered_set>

ECS_BEGIN_NS

template<typename C, i32 size>
class ComponentPool {
public:

private:

    std::array<C, size> components;

};

ECS_END_NS