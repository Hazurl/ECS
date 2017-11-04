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

    using container_t = SparseSet<C, size, E, grow_policy>;
    using forward_iterator = typename container_t::forward_iterator;
    using backward_iterator = typename container_t::backward_iterator;
    using forward_const_iterator = typename container_t::forward_const_iterator;
    using backward_const_iterator = typename container_t::backward_const_iterator;

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
    backward_iterator rbegin(){ return pool.rbegin(); }
    backward_iterator rend()  { return pool.rend(); }

    forward_const_iterator cbegin() const   { return pool.cbegin(); }
    forward_const_iterator cend() const     { return pool.cend(); }
    backward_const_iterator crbegin() const { return pool.crbegin(); }
    backward_const_iterator crend() const   { return pool.crend(); }

private:

    container_t pool;
    
};

ECS_END_NS