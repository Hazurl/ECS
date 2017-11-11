#pragma once

#include <ecs/Config.hpp>

#include <mtp/Utils.hpp>

ECS_BEGIN_NS

template<typename A, typename T, bool is_forward = true>
class ConstRandomIterator { 
public:
    using difference_type = std::ptrdiff_t;
    using ret_type_const_ptr = const T*;
    using ret_type_const_ref = const T&;
    using type_ref = const A&;
    using type_ptr = const A*;
    using iterator_category = std::random_access_iterator_tag;

    using this_t = ConstRandomIterator<A, T, is_forward>;
    
    ConstRandomIterator(type_ref a, ui32 pos) : a(&a), pos(pos) {}
    ConstRandomIterator() : a(nullptr), pos(0) {}
    ConstRandomIterator(const this_t& it) : a(it.a), pos(it.pos) {}
    ~ConstRandomIterator() = default;

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

    difference_type operator-(this_t const& it) const   { return pos - it.pos; }

    ret_type_const_ref operator*() const         { return (*a)[actual()]; }
    ret_type_const_ptr operator->() const        { return &*(*this); }
    ret_type_const_ref operator[](ui32 p) const  { return *((*this) + p); }
    
private:
    
    ui32 move(ui32 ofst)         { return pos = offset(ofst); }
    ui32 offset(ui32 ofst) const { return pos + ofst; }
    ui32 actual() const          { return is_forward ? pos : (a->size() - pos - 1); }

    type_ptr a;
    ui32 pos;
};

template<typename A, typename T, bool is_forward = true>
class RandomRefIterator { 
public:
    using difference_type = std::ptrdiff_t;
    using ret_type_ref = T&;
    using ret_type_ptr = T*;
    using ret_type_const_ptr = const T*;
    using ret_type_const_ref = const T&;
    using type_ref = A&;
    using type_ptr = A*;
    using iterator_category = std::random_access_iterator_tag;

    using this_t = RandomRefIterator<A, T, is_forward>;
    
    RandomRefIterator(type_ref a, ui32 pos) : a(&a), pos(pos) {}
    RandomRefIterator() : a(nullptr), pos(0) {}
    RandomRefIterator(const this_t& it) : a(it.a), pos(it.pos) {}
    ~RandomRefIterator() = default;

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

    difference_type operator-(this_t const& it) const   { return pos - it.pos; }

    ret_type_const_ref operator*() const         { return (*a)[actual()]; }
    ret_type_const_ptr operator->() const        { return &*(*this); }
    ret_type_const_ref operator[](ui32 p) const  { return *((*this) + p); }

    ret_type_ref operator*()       { return (*a)[actual()]; }
    ret_type_ptr operator->()      { return &*(*this); }
    ret_type_ref operator[](ui32 p){ return *((*this) + p); }

    operator ConstRandomIterator<A, T, is_forward> () const { return a == nullptr ? ConstRandomIterator<A, T, is_forward>() : ConstRandomIterator<A, T, is_forward>(*a, pos); }
    
private:
    
    ui32 move(ui32 ofst)         { return pos = offset(ofst); }
    ui32 offset(ui32 ofst) const { return pos + ofst; }
    ui32 actual() const          { return is_forward ? pos : (a->size() - pos - 1); }

    type_ptr a;
    ui32 pos;
};

ECS_END_NS