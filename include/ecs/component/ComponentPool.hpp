#pragma once

#include <ecs/Config.hpp>
#include <ecs/tools/SparseSet.hpp>

#include <cassert>
#include <utility>
#include <iostream>

ECS_BEGIN_NS

template<typename C, i32 size, typename grow_policy = instant_grow_policy>
class ComponentPool {
public:

    void reset() {
        return pool.reset();
    }

    bool has(i32 entity) {
        return pool.has(entity);
    }

    C& get(i32 entity) {
        return pool.get(entity);
    }

    const C& get(i32 entity) const {
        return pool.get(entity);
    }

    template<typename...Args>
    void add(i32 entity, Args&&... args) {
        pool.add(entity, std::forward<Args>(args)...);
    }

    void remove(i32 entity) {
        pool.remove(entity);
    }

    template<typename F>
    void for_each(F f) {
        pool.for_each(f);
    }

    template<typename F>
    void for_each(F f) const {
        pool.for_each(f);
    }

private:

    SparseSet<C, size, grow_policy> pool;
    
};

ECS_END_NS