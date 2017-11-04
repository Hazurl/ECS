#pragma once

#include <ecs/Config.hpp>
#include <ecs/view/View.hpp>
#include <ecs/container/iterators/RandomRefIterator.hpp>

#include <vector>

ECS_BEGIN_NS

template<template<typename> typename Pool, typename Entity, typename...Cs>
class Views {
public:
    using this_views_t = Views<Pool, Entity, Cs...>;

    template<typename It, bool is_forward = true>
    class Iterator_t { 
    public:
        using difference_type = std::ptrdiff_t;
        using ret_type = View<Entity, Cs...>;
        using const_ret_type = View<Entity, const Cs...>;
        using iterator_category = std::bidirectional_iterator_tag;
    
        using this_t = Iterator_t<It, is_forward>;
    
        Iterator_t(this_views_t& a, It it) : a(&a), it(it) {}
        Iterator_t() : a(nullptr), it() {}
        Iterator_t(const this_t& o) : a(o.a), it(o.it) {}
        ~Iterator_t() = default;
    
        //template<bool is_forw>
        //operator ConstIterator_t<is_forw> () const { return a == nullptr ? ConstIterator_t<is_forw>() : ConstIterator_t<is_forw>(*a, pos); }
    
        this_t& operator=(const this_t& o)     { it = o.it; a = o.a; return *this; }
    
        bool operator==(const this_t& o) const { return a == o.a && it == o.it; }
        bool operator!=(const this_t& o) const { return a != o.a || it != o.it; }
        bool operator<(const this_t& o) const  { return it < o.it; }
        bool operator>(const this_t& o) const  { return it > o.it; }
        bool operator<=(const this_t& o) const { return it <= o.it; }
        bool operator>=(const this_t& o) const { return it >= o.it; }
    
        this_t& operator++()                                { move(1); return *this; }
        this_t operator++(int)                              { auto old = *this; move(1); return old; }
        this_t& operator+=(ui32 p)                          { move(p); return *this; }
    
        this_t& operator--()                                { move(-1); return *this; }
        this_t operator--(int)                              { auto old = *this; move(-1); return old; }
        this_t& operator-=(ui32 p)                          { move(-p); return *this; }     
    
        difference_type operator-(this_t const& o) const   { return it - o.it; }
    
        const_ret_type operator*() const {
            auto ent = (*it).first;
            return { ent, std::tuple<Cs*...>( &std::get<Pool<Cs>&>(a->pools).get(ent)... ) };
        }
        ret_type operator*() {
            auto ent = (*it).first;
            return { ent, std::tuple<Cs*...>( &std::get<Pool<Cs>&>(a->pools).get(ent)... ) };
        }
    
    private:
    
        void move(ui32 ofst) {
            auto& main_pool = std::get<Pool<mtp::first<mtp::List<Cs...>>>&>(a->pools);
            for (ui32 of = 0; of < ofst && ++it != main_pool.end();) {
                bool has_all = true;
                mtp::apply_lambda<mtp::List<Cs...>>{}([&] (auto x) {
                    using T = mtp::type_of<decltype(x)>;
                    has_all = has_all && std::get<Pool<T>&>(a->pools).has((*it).first);
                });
                if (has_all)
                    of++;
            }
        }
    
        this_views_t* a;
        It it;
    };    

    using this_t = Views<Pool, Entity, Cs...>;
    using forward_iterator = Iterator_t<decltype(std::declval<Pool<mtp::first<mtp::List<Cs...>>>>().begin()), true>;
    //using backward_iterator = Iterator<this_t, View<Entity, Cs...>, false>;
    //using forward_const_iterator = ConstIterator<this_t, View<Entity, Cs...>, true>;
   // using backward_const_iterator = ConstIterator<this_t, View<Entity, Cs...>, false>;

    Views(std::tuple<Pool<Cs>&...> pools) : pools(pools), main_pool(std::get<Pool<mtp::first<mtp::List<Cs...>>>&>(pools)) {}
    ~Views() = default;

    forward_iterator begin()  { return forward_iterator(*this, main_pool.begin()); }
    forward_iterator end()    { return forward_iterator(*this, main_pool.end()); }
    //backward_iterator rbegin(){ return backward_iterator(*this, 0); }
    //backward_iterator rend()  { return backward_iterator(*this, size()); }

    //forward_const_iterator cbegin() const   { return forward_const_iterator(*this, 0); }
    //forward_const_iterator cend() const     { return forward_const_iterator(*this, size()); }
    //backward_const_iterator crbegin() const { return backward_const_iterator(*this, 0); }
    //backward_const_iterator crend() const   { return backward_const_iterator(*this, size()); }

private:

    std::tuple<Pool<Cs>&...> pools;
    Pool<mtp::first<mtp::List<Cs...>>>& main_pool;

};

ECS_END_NS