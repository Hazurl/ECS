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

    using ArgsList = ArgsGetter<mtp::flatten<
                                mtp::transform<
                                mtp::transform<
                                    typename Context::systems, mtp::type_of
                                >, SystemMethods
                                >>>;

    mtp::ShowType<ArgsList> __s;
    
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
    void update(Args&&...args) {
        mtp::apply_lambda<typename Context::systems_type>{}([&] (auto x) {
            using T = typename decltype(x)::type;
            this->update_system<T>();
        });
    }

//private:

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
    void update_system() {
        mtp::apply_lambda<SystemMethods<S>>{}([&] (auto x) {
            using M = mtp::type_of<decltype(x)>;
            this->update_system_method<S, typename M::type, M::function>();
        });
    }

    template<typename S, typename F, F f>
    void update_system_method() {
        MethodCaller<S, F>::call(get_system<S>(), f, MethodCallerHelper<Entity_t, Controller<Context>>{this});
    }

    friend MethodCallerHelper<Entity_t, Controller<Context>>;

    template<typename...ArgsInView>
    Views<Entity_t, ArgsInView...> construct_views () {
        static_assert(mtp::unique_v<mtp::List<ArgsInView...>>, "Components in the view must be uniques");
        using Vw = View<Entity_t, ArgsInView...>;
        using TupleVw = std::tuple<ArgsInView*...>;
        using Main = mtp::first<mtp::List<ArgsInView...>>;
        auto& main = get_pool<Main>();

        typename Views<Entity_t, ArgsInView...>::template container_t<Vw> container {};
        for (auto& ent_id : main.keys()) {
            TupleVw tuple;
            std::get<Main*>(tuple) = &main.get(ent_id);
            bool has_failed = false;
            mtp::apply_lambda<mtp::erase_front<mtp::List<ArgsInView...>>>{}([this, &has_failed, &tuple, ent_id] (auto x) {
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
        return Views<Entity_t, ArgsInView...>{ container };
    }

    SystemsList systems;
    PoolList componentPools;
    EntityManager_t entityManager;
    
};

ECS_END_NS