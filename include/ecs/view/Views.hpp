#pragma once

#include <ecs/Config.hpp>
#include <ecs/view/View.hpp>
#include <vector>
#include <ecs/container/iterators/RandomRefIterator.hpp>

#include <vector>

ECS_BEGIN_NS

template<typename Entity, typename...Cs>
class Views {
    template<typename T>
    using container_t = std::vector<T>;

public:

    Views(container_t<View<Entity, Cs...>> & views) : views(views) {}
    ~Views() = default;

    auto begin()  { return views.begin(); }
    auto end()    { return views.end(); }
    auto begin() const { return views.begin(); }
    auto end() const   { return views.end(); }
    auto cbegin() const   { return views.cbegin(); }
    auto cend() const     { return views.cend(); }

private:

    container_t<View<Entity, Cs...>> & views;

};

ECS_END_NS