#pragma once

#include <ecs/Config.hpp>

#include <mtp/list/List.hpp>

#include <functional>

ECS_BEGIN_NS

template<typename E, typename...ViewComponents>
class View {
    using ViewComponentsList = mtp::List<ViewComponents...>;

    static_assert(mtp::unique_v<ViewComponentsList>, "Components must be unique in a view");

public:

    View(E ent, std::tuple<ViewComponents*...> const& components) : ent(ent), components(components) {}
    ~View() = default;

    E entity() const { return ent; }

    template<typename C, typename = std::enable_if_t<mtp::count_v<ViewComponentsList, C> == 1>>
    C& get() {
        return *std::get<C*>(components);
    }

    template<typename C, typename = std::enable_if_t<mtp::count_v<ViewComponentsList, C> == 1>>
    const C& get() const {
        return *std::get<C*>(components);
    }

private:

    E ent;
    std::tuple<ViewComponents*...> components;

};

ECS_END_NS