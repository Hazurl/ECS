#pragma once

#include <ecs/Config.hpp>
#include <ecs/component/ComponentPool.hpp>
#include <ecs/entity/EntityManager.hpp>
#include <ecs/container/Tuple.hpp>
#include <ecs/controller/context/Context.hpp>
#include <ecs/view/Views.hpp>
#include <ecs/controller/MethodCaller.hpp>
#include <ecs/controller/context/ArgsGetter.hpp>

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

    template<typename S>
    using SystemMethods = typename mtp::at<typename Context::systems, mtp::index_of_v<typename Context::systems_type, S>>::methods;
    using SystemsList = Tuple<typename Context::systems_type>;

    using ArgsList = 
        mtp::transform<
            ArgsGetter< 
                mtp::flatten< 
                    mtp::transform< 
                        mtp::transform< 
                            typename Context::systems, mtp::type_of 
                        >, SystemMethods 
                    >
                >
            >, mtp::remove_qualifiers
        >;
    
public:
    using Entity_t = typename EntityManager_t::Entity_t;

    template<typename SC>
    Controller(SC sc) {
        mtp::apply_lambda<typename Context::systems_type>{}([&] (auto s) {
            using S = typename decltype(s)::type;
            this->systems.template construct<S>(sc.template construct_system<S>());
        });
    }

    Controller() : Controller(SystemsConstructor<Systems_list<>>{}) {}

    inline Entity_t create () { 
        return entityManager.create();
    }

    inline void destroy (Entity_t ent) { 
        reset_components(ent);
        entityManager.destroy(ent);
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

    template<typename...Args>
    void update(Args&&...args) {/*
        mtp::apply_lambda<typename Context::systems_type>{}([&] (auto x) {
            using T = typename decltype(x)::type;
            this->update_system<T>();
        });*/
        update_args(std::forward<Args>(args)...);
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

    template<typename Arg, typename...Args>
    void update_args(Arg&& arg, Args&&...args) {
        update_args(std::forward<Args>(args)...);
        update_arg(std::forward<Arg>(arg));
    }

    void update_args() {
        update_views_args();
    }

    template<typename Arg>
    void update_arg(Arg&& arg) {
        if /* constexpr */ (mtp::count_v<ArgsList, Arg> > 0) {
            all_args.template reconstruct<Arg>(std::forward<Arg>(arg));
        }
    }

    void update_views_args() {
        mtp::apply_lambda<ArgsList>{}([this] (auto x) {
            using X = mtp::type_of<decltype(x)>;
            this->update_views_arg<X>(is_views<mtp::remove_qualifiers<X>>{});
        });
    }

    template<typename V>
    void update_views_arg(mtp::True) {
        all_args.template reconstruct<V>(construct_views<V>());
    }

    template<typename V>
    void update_views_arg(mtp::False) {}

    template<typename S>
    void update_system() {
        mtp::apply_lambda<SystemMethods<S>>{}([&] (auto x) {
            using M = mtp::type_of<decltype(x)>;
            this->update_system_method<S, typename M::type, M::function>();
        });
    }

    template<typename S, typename F, F f>
    void update_system_method() {
        //MethodCaller<S, F>::call(get_system<S>(), f, MethodCallerHelper<Entity_t, Controller<Context>>{this});
    }

    template<typename V, typename List_Views_Args = typename V::list_t>
    V construct_views () {
        static_assert(mtp::unique_v<List_Views_Args>, "Components in the view must be uniques");
        using TupleVw = mtp::as_tuple<mtp::transform<List_Views_Args, mtp::add_ptr>>;
        using Main = mtp::first<List_Views_Args>;
        auto& main = get_pool<Main>();

        typename V::container_t container {};
        for (auto& ent_id : main.keys()) {
            TupleVw tuple;
            std::get<Main*>(tuple) = &main.get(ent_id);
            bool has_failed = false;
            mtp::apply_lambda<mtp::erase_front<List_Views_Args>>{}([this, &has_failed, &tuple, ent_id] (auto x) {
                using A = mtp::type_of<decltype(x)>;
                auto& pool = this->get_pool<A>();
                if (pool.has(ent_id))
                    std::get<A*>(tuple) = &pool.get(ent_id);
                else
                    has_failed = true;
            });

            if (!has_failed) {
                container.emplace_back(entityManager.sync(ent_id), std::move(tuple));
            }
        }
        return V{ container };
    }

    SystemsList systems;
    PoolList componentPools;
    EntityManager_t entityManager;

    Tuple<ArgsList> all_args;
    
};

ECS_END_NS