#pragma once

#include <ecs/Config.hpp>
#include <ecs/container/SparseSet.hpp>

#include <cassert>
#include <utility>
#include <iostream>

ECS_BEGIN_NS

template<typename C, i32 Size, typename E, typename grow_policy = instant_grow_policy>
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
    C& add(E entity, Args&&... args) {
        pool.add(entity, std::forward<Args>(args)...);
        return get(entity);
    }

    void remove(E entity) {
        pool.remove(entity);
    }

    ui32 size() const {
        return pool.size();
    }

    auto begin()        { return pool.begin(); }
    auto end()          { return pool.end(); }
    auto cbegin() const { return pool.cbegin(); }
    auto cend() const   { return pool.cend(); }
    auto keys () const  { return pool.keys(); }
    
private:

    SparseSet<C, Size, E, grow_policy> pool;
    
};

ECS_END_NS