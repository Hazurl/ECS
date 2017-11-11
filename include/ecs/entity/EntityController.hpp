#pragma once

#include <ecs/Config.hpp>

#include <ecs/entity/Entity.hpp>
#include <ecs/entity/EntityManager.hpp>
#include <ecs/component/ComponentPool.hpp>

#include <mtp/list/List.hpp>

#include <functional>
#include <vector>

ECS_BEGIN_NS

template<typename _Entity, typename _Components, typename _limit_size, typename _grow_policy>
class EntityController {
public:

    using this_t = EntitiesController<_Entity, _Components, _limit_size, _grow_policy>;

    struct Futur {
        using Func = std::function<void(this_t&)>;

        Futur& operator += (Func func) {
            funcs.push_back(func);
            return *this;
        }

    private:
        template<typename S, typename C>
        friend SystemUpdater<S, C>;

        void execute(this_t& controller) {
            for(auto& func : funcs)
                func(controller);
            funcs.clear();
        }

        std::vector<Func> funcs;
    };

    using Entity_t = _Entity;
    using Components = _Components;
    using grow_policy = _grow_policy;
    using user_bridge = Futur;
    static constexpr ui32 limit_size = _limit_size::value;

private:

    using EntityManager_t = EntityManager<Entity_t>;

    template<typename C>
    using Pool = ComponentPool<C, limit_size, typename Entity_t::T_id, grow_policy>;
    using Pools = mtp::transform<Components, Pool>;

public:

    Entity_t create () { 
        return entityManager.create();
    }

    void destroy (Entity_t ent) { 
        if (entityManager.has(ent)) {
            reset_components(ent);
            entityManager.destroy(ent);
        }
    }

    bool alive (Entity_t ent) const { 
        return entityManager.alive(ent);
    }

    ui32 size () const { 
        return entityManager.size();
    }

    bool empty () const { 
        return entityManager.empty();
    }

    template<typename...Comps>
    void remove_components(Entity_t ent) {
        mtp::apply_lambda<
        mtp::switch_t<mtp::empty_v<mtp::List<Comps...>>, // if Comps is empty, then reset all components
                      typename Context::components_type, 
                      mtp::List<Comps...>>>{}([&] (auto x) {
            using T = typename decltype(x)::type;
            this->std::get<Pool<T>>(pools).remove(ent.id());
        });
    }

    template<typename C>
    bool has_component(Entity_t ent) {
        return std::get<Pool<C>>(pools).has(ent.id());        
    }

    template<typename C>
    C& get_component(Entity_t ent) {
        return std::get<Pool<C>>(pools).get(ent.id());
    }

    template<typename C>
    const C& get_component(Entity_t ent) const {
        return std::get<Pool<C>>(pools).get(ent.id());
    }

    template<typename C, typename...Args>
    C& add_component(Entity_t ent, Args&&... args) {
        return std::get<Pool<C>>(pools).add(ent.id(), std::forward<Args>(args)...);
    }

    template<typename C, typename...Args>
    C& ensure_component(Entity_t ent, Args&&... args) {
        return has_component<C>(ent) ? 
            get<C>(ent) : 
            std::get<Pool<C>>(pools).add(ent.id(), std::forward<Args>(args)...);
    }

private:

    EntityManager_t entityManager;
    mtp::as_tuple<Pools> pools;

};

ECS_END_NS