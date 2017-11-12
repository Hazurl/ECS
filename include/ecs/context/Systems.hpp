#pragma once

#include <ecs/Config.hpp>

#include <mtp/list/List.hpp>

#include <functional>
#include <tuple>

ECS_BEGIN_NS

template<typename F, F f>
struct Method {
    using type = F;
    static constexpr F function = f;
};

template<typename T, typename M>
struct System {
    using type = T;
    using methods = M;
};

template<typename S>
struct TransformSystemHelper : mtp::TConst<System<S, mtp::List< Method<decltype(&S::update), (&S::update)> >>> {};

template<typename S, typename M>
struct TransformSystemHelper<System<S, M>> : mtp::TConst<System<S, M>> {};

template<typename S>
using TransformSystem = typename TransformSystemHelper<S>::type;

template<typename...Srgs>
class SystemsConstructor : std::tuple<std::function<Srgs()>...> {
    template<typename S> using F = std::function<S()>;

    using L = mtp::List<Srgs...>;
    static_assert(mtp::unique_v<L>, "Systems must be unique");
    
public:

    template<typename S>
    static constexpr bool has_constructor_for = mtp::count_v<L, S> > 0;

    SystemsConstructor(std::function<Srgs()>...funcs) 
    : std::tuple<std::function<Srgs()>...>(std::forward<std::function<Srgs()>>(funcs)...) {}

    template<typename S>
    std::enable_if_t<has_constructor_for<S>,
    S> construct_system() const {
        return std::get<F<S>>(*this)();
    }

    template<typename S>
    std::enable_if_t<!has_constructor_for<S>,
    S> construct_system() const {
        return S{};
    }
};

ECS_END_NS