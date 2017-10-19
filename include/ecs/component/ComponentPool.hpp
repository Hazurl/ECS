#pragma once

#include <ecs/Config.hpp>

#include <cassert>
#include <utility>
#include <iostream>

ECS_BEGIN_NS

template<typename C, i32 size>
class ComponentPool {
public:

    ComponentPool() {
        std::fill(comp_entity_map, comp_entity_map + size, -1);
        std::fill(entity_comp_map, entity_comp_map + size, -1);
    }

    ~ComponentPool() {
        reset();
    }

    void reset() {
        auto comps = reinterpret_cast<C*>(components);
        for(int i = 0; i < used; ++i)
            comps[i].~C();
        std::fill(comp_entity_map, comp_entity_map + size, -1);
        std::fill(entity_comp_map, entity_comp_map + size, -1);
    }

    bool has(i32 entity) {
        assert(entity >= 0);
        return entity_comp_map[entity] != -1;
    }

    C& get(i32 entity) {
        assert(has(entity));
        return reinterpret_cast<C*>(components)[entity_comp_map[entity]];
    }

    const C& get(i32 entity) const {
        assert(has(entity));
        return reinterpret_cast<const C*>(components)[entity_comp_map[entity]];
    }

    template<typename...Args>
    void add(i32 entity, Args&&... args) {
        assert(!has(entity));
        entity_comp_map[entity] = used;
        comp_entity_map[used] = entity;
        new (&reinterpret_cast<C*>(components)[used++]) C(std::forward<Args>(args)...);
    }

    void remove(i32 entity) {
        assert(has(entity));
        i32 cidx = entity_comp_map[entity];
        auto comps = reinterpret_cast<C*>(components);
        comps[cidx].~C();
        
        if (used > 1) { // move last
            i32 last = comp_entity_map[used - 1];
            if (last != entity) {
                comps[cidx] = std::move(comps[used - 1]);
                comps[used - 1].~C();

                entity_comp_map[entity] = cidx;
                comp_entity_map[cidx] = last;
                entity_comp_map[last] = -1;
                comp_entity_map[used - 1] = -1;
            }
        } else {
            entity_comp_map[entity] = comp_entity_map[cidx] = -1;            
        }

        --used;
    }

    template<typename F>
    void for_each(F f) {
        auto comps = reinterpret_cast<C*>(components);
        for (int i = 0; i < used; ++i)
            f(comps[i]);
    }

    template<typename F>
    void for_each(F f) const {
        auto comps = reinterpret_cast<const C*>(components);
        for (int i = 0; i < used; ++i)
            f(comps[i]);
    }

private:

    ui8 components[size * sizeof(C)];
    i32 comp_entity_map[size];
    i32 entity_comp_map[size];
    i32 used = 0;

};

ECS_END_NS