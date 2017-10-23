#pragma once

#include <ecs/Config.hpp>
#include <ecs/container/SparseSet.hpp>

#include <cassert>
#include <utility>
#include <iostream>

ECS_BEGIN_NS

template<typename C, i32 size, typename E, typename grow_policy = instant_grow_policy>
class ComponentPool {
public:

    void reset() {
        return pool.reset();
    }

    bool has(E entity) {
        return pool.has(entity);
    }

    C& get(E entity) {
        return pool.get(entity);
    }

    const C& get(E entity) const {
        return pool.get(entity);
    }

    template<typename...Args>
    void add(E entity, Args&&... args) {
        pool.add(entity, std::forward<Args>(args)...);
    }

    void remove(E entity) {
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

    SparseSet<C, size, E, grow_policy> pool;
    
};

ECS_END_NS