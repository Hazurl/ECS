#pragma once

#include <ecs/Config.hpp>
#include <ecs/component/ComponentPool.hpp>
#include <ecs/entity/EntityManager.hpp>

#include <mtp/Utils.hpp>
#include <mtp/list/List.hpp>

#include <tuple>

ECS_BEGIN_NS

template<typename Components, typename Systems, typename Context>
class Controller {
    static_assert(mtp::AlwaysFalse<Components, Systems>::value, "Use Components_list, Systems_list and Context ");
};

template<typename...Args>
using Components_list = mtp::List<Args...>;

template<typename...Args>
using Systems_list = mtp::List<Args...>;

template<typename...Components, typename...Systems, typename Context>
class Controller<Components_list<Components...>, Systems_list<Systems...>, Context> {
    using CompList = Components_list<Components...>;
    using SysList = Systems_list<Systems...>;

    using EntityManager_t = EntityManager<typename Context::entity_mask>;
    template<typename C> 
    using Pool = ComponentPool<C, Context::pool_size, typename EntityManager_t::Entity_t::T_id, typename Context::pool_grow_policy>;
    using PoolList = mtp::as_tuple<mtp::transform<CompList, Pool>>;
    using SystemsList = mtp::as_tuple<SysList>;

    using Time_t = typename Context::Time_t;
    
public:

    using Entity_t = typename EntityManager_t::Entity_t;

    inline Entity_t create () { 
        return entityManager.create();
    }

    inline void destroy (Entity_t ent) { 
        return entityManager.destroy(ent.id());
    }

    inline bool alive (Entity_t ent) const { 
        return entityManager.alive(ent.id());
    }

    inline ui32 size () const { 
        return entityManager.size();
    }

    inline bool empty () const { 
        return entityManager.empty();
    }

    template<typename...Comps>
    inline void reset_components(Entity_t ent) {
        mtp::apply_lambda<CompList>{}([&] (auto x) {
            using T = typename decltype(x)::type;
            this->get_pool<T>().remove(ent.id());
        });
    }

    template<typename C>
    bool has_component(Entity_t ent) {
        return get_pool<C>().has(ent.id());        
    }

    template<typename C>
    C& get(Entity_t ent) {
        return get_pool<C>().get(ent.id());
    }

    template<typename C>
    const C& get(Entity_t ent) const {
        return get_pool<C>().get(ent.id());
    }

    template<typename C, typename...Args>
    void add(Entity_t ent, Args&&... args) {
        return get_pool<C>().add(ent.id(), std::forward<Args>(args)...);
    }

    template<typename C, typename...Args>
    void ensure(Entity_t ent, Args&&... args) {
        return has_component<C>(ent) ? get<C>(ent) : get_pool<C>().add(ent.id(), std::forward<Args>(args)...);
    }

    template<typename C>
    void remove(Entity_t ent) {
        return get_pool<C>().remove(ent.id());
    }

    void update(Time_t const& t) {
        mtp::apply_lambda<SystemsList>{}([&] (auto x) {
            using T = typename decltype(x)::type;
            this->get_system<T>().update(t);
        });
    }

private:

    template<typename C>
    Pool<C>& get_pool() {
        return std::get<Pool<C>>(componentPools);
    }

    template<typename C>
    const Pool<C>& get_pool() const {
        return std::get<Pool<C>>(componentPools);
    }
    template<typename C>
    Pool<C>& get_system() {
        return std::get<Pool<C>>(systems);
    }

    template<typename C>
    const Pool<C>& get_system() const {
        return std::get<Pool<C>>(systems);
    }

    SystemsList systems;
    PoolList componentPools;
    EntityManager_t entityManager;
    
};

ECS_END_NS