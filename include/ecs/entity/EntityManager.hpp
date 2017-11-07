#pragma once

#include <ecs/Config.hpp>
#include <ecs/entity/Entity.hpp>

#include <vector>

ECS_BEGIN_NS

template<typename entity_policy>
class EntityManager {
public:

    using Entity_t = Entity<entity_policy>;

    Entity_t create() {
        if (free_list.empty()) {
            auto ent = Entity_t::create(entities.size(), 0);
            entities.push_back(ent);
            return ent;
        } else {
            auto ent = free_list.back();
            free_list.pop_back();
            return ent;
        }
    }

    void destroy(Entity_t ent) {
        if (alive(ent)) {
            auto next = Entity_t::next_version(ent);
            free_list.push_back(next);
            entities[ent.id()] = next;
        }
    }

    bool alive(Entity_t ent) const {
        auto id = ent.id();
        return entities.size() > id && entities[id] == ent;
    }

    inline ui32 size() const {
        return entities.size() - free_list.size();
    }

    inline bool empty() const {
        return size() == 0;
    }

    Entity_t sync(typename Entity_t::T_id id) const {
        return entities[id];
    }

private:

    std::vector<Entity_t> entities;
    std::vector<Entity_t> free_list;
    
};

ECS_END_NS