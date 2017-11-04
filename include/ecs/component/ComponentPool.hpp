#pragma once

#include <ecs/Config.hpp>
#include <ecs/container/SparseSet.hpp>

#include <cassert>
#include <utility>
#include <iostream>

ECS_BEGIN_NS

template<typename C, i32 size, typename E, typename grow_policy = instant_grow_policy>
class ComponentPool {

    using container_t = SparseSet<C, size, E, grow_policy>;

public:

    using forward_iterator = typename container_t::forward_iterator;
    using forward_const_iterator = typename container_t::forward_const_iterator;

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

    forward_iterator begin()  { return pool.begin(); }
    forward_iterator end()    { return pool.end(); }
    forward_const_iterator cbegin() const   { return pool.cbegin(); }
    forward_const_iterator cend() const     { return pool.cend(); }

private:

    container_t pool;
    
};

ECS_END_NS