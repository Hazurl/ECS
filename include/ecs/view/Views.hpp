#pragma once

#include <ecs/Config.hpp>
#include <ecs/view/View.hpp>
#include <vector>
#include <ecs/container/iterators/RandomRefIterator.hpp>

#include <vector>

ECS_BEGIN_NS

template<typename Entity, typename...Cs>
class Views {
public:

    using view_t = View<Entity, Cs...>;
    using container_t = std::vector<view_t>;
    using as_list = mtp::List<Cs...>;

    Views(container_t const& views) : views(views) {}
    ~Views() = default;

    auto begin()  { return views.begin(); }
    auto end()    { return views.end(); }
    auto begin() const { return views.begin(); }
    auto end() const   { return views.end(); }
    auto cbegin() const   { return views.cbegin(); }
    auto cend() const     { return views.cend(); }

private:

    container_t views;

};

template<typename V>
struct is_views_helper : mtp::False {
    //mtp::ShowType<V> k;
};

template<typename E, typename...Cs>
struct is_views_helper<Views<E, Cs...>> : mtp::True {};

template<typename V>
static constexpr bool is_views = is_views_helper<V>::value;

ECS_END_NS