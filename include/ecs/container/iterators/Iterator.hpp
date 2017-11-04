#pragma once

#include <ecs/Config.hpp>
#include <ecs/container/iterators/ConstIterator.hpp>

ECS_BEGIN_NS

template<typename A, typename T, bool is_forward = true>
class Iterator { 
public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using iterator_category = std::random_access_iterator_tag;

    using this_t = Iterator<A, T, is_forward>;

    Iterator(A& a, ui32 pos) : a(&a), pos(pos) {}
    Iterator() : a(nullptr), pos(0) {}
    Iterator(const this_t& it) : a(it.a), pos(it.pos) {}
    ~Iterator() = default;

    template<bool is_forw>
    operator ConstIterator<A, T, is_forw> () const { return a == nullptr ? ConstIterator<A, T, is_forw>() : ConstIterator<A, T, is_forw>(*a, pos); }

    this_t& operator=(const this_t& it)     { pos = it.pos; a = it.a; return *this; }

    bool operator==(const this_t& it) const { return a == it.a && pos == it.pos; }
    bool operator!=(const this_t& it) const { return a != it.a || pos != it.pos; }
    bool operator<(const this_t& it) const  { return pos < it.pos; }
    bool operator>(const this_t& it) const  { return pos > it.pos; }
    bool operator<=(const this_t& it) const { return pos <= it.pos; }
    bool operator>=(const this_t& it) const { return pos >= it.pos; }

    this_t& operator++()                                { move(1); return *this; }
    this_t operator++(int)                              { auto old = *this; move(1); return old; }
    this_t& operator+=(ui32 p)                          { move(p); return *this; }
    this_t operator+(ui32 p) const                      { return this_t(*a, offset(p)); }
    friend this_t operator+(ui32 p, const this_t& it)   { return it + p; }

    this_t& operator--()                                { move(-1); return *this; }
    this_t operator--(int)                              { auto old = *this; move(-1); return old; }
    this_t& operator-=(ui32 p)                          { move(-p); return *this; }     
    this_t operator-(ui32 p) const                      { return this_t(*a, offset(-p)); }
    friend this_t operator-(ui32 p, const this_t& it)   { return it - p; }

    difference_type operator-(this_t const& it) const   { return pos - it.pos; }

    const reference operator*() const         { return (*a)[actual()]; }
    pointer operator->() const                { return &*(*this); }
    const reference operator[](ui32 p) const  { return *((*this) + p); }

    reference operator*()                     { return (*a)[actual()]; }
    reference operator[](ui32 p)              { return *((*this) + p); }

private:

    ui32 move(ui32 ofst)    { return pos = offset(ofst); }
    ui32 offset(ui32 ofst)  { return pos + ofst; }
    ui32 actual()           { return is_forward ? pos : (a->size() - pos - 1); }

    A* a;
    ui32 pos;
};

ECS_END_NS