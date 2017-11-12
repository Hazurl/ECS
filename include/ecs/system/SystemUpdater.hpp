#pragma once

#include <ecs/system/ArgsGetter.hpp>
#include <ecs/system/MethodCaller.hpp>
#include <ecs/Config.hpp>
#include <ecs/container/Tuple.hpp>

ECS_BEGIN_NS

template<typename _Systems, typename _EntityController>
class SystemUpdater {
public:

    using Systems = _Systems;
    using Systems_t = mtp::transform<Systems, mtp::type_of>;
    using EntityController_t = _EntityController;
    using UserEntityController = typename EntityController_t::user_bridge;

private:
    template<typename C>
    using Pool = typename EntityController_t::template Pool<C>;

    using Systems_tuple = Tuple_from_list<Systems_t>;
    template<typename S>
    using UpdateMethodsOfSystem = typename S::methods;
    template<typename S>
    using UpdateMethodsOf = UpdateMethodsOfSystem<mtp::at<Systems, mtp::index_of_v<Systems_t, S>>>;

    using ArgsList = 
        mtp::transform<
            ArgsGetter< 
                mtp::flatten<
                    mtp::transform< 
                        Systems, UpdateMethodsOfSystem
                    >
                >
            >, mtp::remove_qualifiers
        >;
    using Args_tuple = Tuple_from_list<ArgsList>;


public:

    template<typename SystemConstr>
    SystemUpdater(EntityController_t& controller, SystemConstr const& sc = SystemsConstructor<>{}) : controller(controller) {
        mtp::apply_lambda<Systems_t>{}([this, &sc] (auto s) {
            using S = mtp::type_of<decltype(s)>;
            this->systems.template construct<S>(sc.template construct_system<S>());
        });
    }

    template<typename...Args>
    void update(Args&&...args) {
        construct_args(std::forward<Args>(args)...);
        construct_views();
        construct_controller(mtp::in<ArgsList, UserEntityController>{});
        update_systems();
        update_controller(mtp::in<ArgsList, UserEntityController>{});
    }

private:

    void update_controller(mtp::False) {}
    void update_controller(mtp::True) {
        syst_args.template get<UserEntityController>().execute(controller);
    }

    void update_systems() {
        mtp::apply_lambda<Systems_t>{}([&] (auto x) {
            using T = typename decltype(x)::type;
            this->update_system<T>();
        });
    }

    template<typename S>
    void update_system() {
        mtp::apply_lambda<UpdateMethodsOf<S>>{}([&] (auto x) {
            using M = mtp::type_of<decltype(x)>;
            MethodCaller<S, typename M::type>::call(this->systems.template get<S>(), M::function, this->syst_args);
        });
    }

    void construct_args() {}
    template<typename Arg, typename...Args>
    void construct_args(Arg&& arg, Args&&...args) {
        construct_arg(std::forward<Arg>(arg), mtp::in<ArgsList, Arg>{});
        construct_args(std::forward<Args>(args)...);
    }

    template<typename Arg>
    void construct_arg(Arg&&, mtp::False) {}
    template<typename Arg>
    void construct_arg(Arg&& arg, mtp::True) {
        syst_args.template reconstruct<mtp::remove_qualifiers<Arg>>(std::forward<Arg>(arg));
    }

    void construct_controller(mtp::False) {}
    void construct_controller(mtp::True) {
        syst_args.template reconstruct<UserEntityController>();
    }

    void construct_views() {
        mtp::apply_lambda<mtp::filter<ArgsList, is_views>>{}([this] (auto v) {
            using V = mtp::type_of<decltype(v)>;
            this->syst_args.template reconstruct<V>(
                this->construct_view<V>()
            );
        });
    }

    template<typename V, typename List_Views_Args = typename V::list_t>
    V construct_view () {
        static_assert(mtp::unique_v<List_Views_Args>, "Components in the view must be uniques");
        using TupleVw = mtp::as_tuple<mtp::transform<List_Views_Args, mtp::add_ptr>>;
        using Main = mtp::first<List_Views_Args>;
        auto& main = std::get<Pool<Main>>(controller.pools);

        typename V::container_t container {};
        for (auto& ent_id : main.keys()) {
            TupleVw tuple;
            std::get<Main*>(tuple) = &main.get(ent_id);
            bool has_failed = false;
            mtp::apply_lambda<mtp::erase_front<List_Views_Args>>{}([this, &has_failed, &tuple, ent_id] (auto x) {
                if (!has_failed) {
                    using A = mtp::type_of<decltype(x)>;
                    auto& pool = std::get<Pool<A>>(controller.pools);
                    if (pool.has(ent_id))
                        std::get<A*>(tuple) = &pool.get(ent_id);
                    else
                        has_failed = true;
                }
            });

            if (!has_failed) {
                container.emplace_back(controller.entityManager.sync(ent_id), std::move(tuple));
            }
        }
        return V{ container };
    }

    EntityController_t& controller;
    Systems_tuple systems;
    Args_tuple syst_args;

};

ECS_END_NS