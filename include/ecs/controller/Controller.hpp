#pragma once

#include <ecs/Config.hpp>
#include <ecs/component/ComponentPool.hpp>
#include <ecs/entity/EntityManager.hpp>
#include <ecs/container/Tuple.hpp>
#include <ecs/controller/context/Context.hpp>
#include <ecs/view/Views.hpp>

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

    using Time_t = typename Context::Time_t;

public:
    using Entity_t = typename EntityManager_t::Entity_t;
/*
private:    

    template<typename C>
    struct DynamicGetter {

        DynamicGetter(Pool<C>& pool, Entity_t ent) : pool(&pool), ent(ent) {}

        C& get () {
            return pool->get(ent.id());
        }
        const C& get() const {
            return pool->get(ent.id());
        }

        Pool<C>* pool;
        Entity_t ent;
    };
    */
public:

    template<typename SC>
    Controller(SC sc) {
        mtp::apply_lambda<typename Context::systems_type>{}([&] (auto s) {
            using S = typename decltype(s)::type;
            if constexpr (SC::template has_constructor_for<S>)
                systems.template construct<S>(sc.template construct_system<S>());
            else
                systems.template construct<S>();
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
            this->get_system<T>().update(t);
        });
    }

    template<typename C>
    Views<Pool, Entity_t, C> views () {
        return { std::tuple<Pool<C>&>( get_pool<C>() ) };
    }

/*
    template<typename C>
    std::vector<StaticView<Entity_t, C>> getView () {
        using V = StaticView<Entity_t, C>;
        auto& pool = get_pool<C>();
        std::vector<V> vect;
        for(auto ent_comp : pool) {
            vect.emplace_back(ent_comp.first, std::make_tuple(&ent_comp.second));
        }
        return vect;
    }

    template<typename C>
    std::vector<DynamicView<DynamicGetter, Entity_t, C>> getViewDyn () {
        using V = DynamicView<DynamicGetter, Entity_t, C>;
        auto& pool = get_pool<C>();
        std::vector<V> vect;
        for(auto ent_comp : pool) {
            vect.emplace_back(ent_comp.first, std::make_tuple(DynamicGetter<C>{pool, ent_comp.first}));
        }
        return vect;
    }
*/
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