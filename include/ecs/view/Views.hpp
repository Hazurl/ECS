#pragma once

#include <ecs/Config.hpp>
#include <ecs/view/View.hpp>
#include <ecs/container/iterators/RandomRefIterator.hpp>

#include <vector>

ECS_BEGIN_NS

template<template<typename> typename Pool, typename Entity, typename...Cs>
class Views {
public:
    using this_t = Views<Pool, Entity, Cs...>;

    Views(std::tuple<Pool<Cs>&...> pools) : pools(pools), main_pool(std::get<Pool<mtp::first<mtp::List<Cs...>>>&>(pools)) {}
    ~Views() = default;
/*
    forward_iterator begin()  { return forward_iterator(*this, main_pool.begin()); }
    forward_iterator end()    { return forward_iterator(*this, main_pool.end()); }
    forward_const_iterator cbegin() const   { return forward_const_iterator(*this, 0); }
    forward_const_iterator cend() const     { return forward_const_iterator(*this, size()); }
*/
private:

    std::tuple<Pool<Cs>&...> pools;
    Pool<mtp::first<mtp::List<Cs...>>>& main_pool;

};

ECS_END_NS