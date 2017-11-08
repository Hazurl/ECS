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
    using list_t = mtp::List<Cs...>;

    Views(container_t const& views) : views(views) {
        std::cout << "Views constructed, size: " << views.size() << std::endl;
    }
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
struct is_views : mtp::False {};

template<typename E, typename...Cs>
struct is_views<Views<E, Cs...>> : mtp::True {};

template<typename V>
static constexpr bool is_views_v = is_views<V>::value;

ECS_END_NS