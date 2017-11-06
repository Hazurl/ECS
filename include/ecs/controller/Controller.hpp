#pragma once

#include <ecs/Config.hpp>
#include <ecs/component/ComponentPool.hpp>
#include <ecs/entity/EntityManager.hpp>
#include <ecs/container/Tuple.hpp>
#include <ecs/controller/context/Context.hpp>
#include <ecs/view/Views.hpp>
#include <ecs/controller/MethodCaller.hpp>

#include <mtp/Utils.hpp>
#include <mtp/list/List.hpp>

#include <tuple>

ECS_BEGIN_NS

template<typename Context>
class Controller {
    using EntityManager_t = EntityManager<typename Context::entities_mask>;

    template<typename C> 
    using Pool = ComponentPool<C, Context::pools_size, typename EntityManager_t::Entity_t::T_id, typename Context::pools_grow_policy>;
    using PoolList = mtp::as_tuple<mtp::transform<typename Context::components_type, Pool>>;

    using SystemsList = Tuple<typename Context::systems_type>;
    template<typename S>
    using SystemMethods = typename mtp::at<typename Context::systems, mtp::index_of_v<typename Context::systems_type, S>>::methods;

    using Time_t = typename Context::Time_t;

public:
    using Entity_t = typename EntityManager_t::Entity_t;

    template<typename SC>
    Controller(SC sc) {
        mtp::apply_lambda<typename Context::systems_type>{}([&] (auto s) {
            using S = typename decltype(s)::type;
            systems.template construct<S>(sc.template construct_system<S>());
        });
    }

    Controller() : Controller(SystemsConstructor<Systems_list<>>{}) {}

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
        mtp::apply_lambda<typename Context::components_type>{}([&] (auto x) {
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

    void update(Time_t t) {
        mtp::apply_lambda<typename Context::systems_type>{}([&] (auto x) {
            using T = typename decltype(x)::type;
            this->update_system<T>(t);
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
    template<typename S>
    S& get_system() {
        return systems.get<S>();
    }

    template<typename S>
    const S& get_system() const {
        return systems.get<S>();
    }

    template<typename S>
    void update_system(Time_t t) {
        mtp::apply_lambda<SystemMethods<S>>{}([&] (auto x) {
            using M = mtp::type_of<decltype(x)>;
            this->update_system_method<S, typename M::type, M::function>(t);
        });
    }

    template<typename S, typename F, F f>
    void update_system_method(Time_t t) {
        MethodCaller<S, F>::call(get_system<S>(), f, MethodCallerHelper{});
    }

    SystemsList systems;
    PoolList componentPools;
    EntityManager_t entityManager;
    
};

ECS_END_NS