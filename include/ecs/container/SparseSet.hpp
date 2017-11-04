#pragma once

#include <ecs/Config.hpp>
#include <ecs/container/UninitializedArray.hpp>

#include <cassert>

ECS_BEGIN_NS

template<typename C, ui32 Size, typename Idx, typename grow_policy = instant_grow_policy>
class SparseSet {
public:

    using this_set_t = SparseSet<C, Size, Idx, grow_policy>;

    template<bool is_forward = true>
    class ConstIterator_t { 
    public:
        using difference_type = std::ptrdiff_t;
        using reference = std::pair<Idx, C&>;
        using const_reference = std::pair<Idx, const C&>;
        using iterator_category = std::random_access_iterator_tag;
    
        using this_t = ConstIterator_t<is_forward>;
    
        ConstIterator_t(this_set_t const& a, ui32 pos) : a(&a), pos(pos) {}
        ConstIterator_t() : a(nullptr), pos(0) {}
        ConstIterator_t(const this_t& it) : a(it.a), pos(it.pos) {}
        ~ConstIterator_t() = default;
    
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
    
        const_reference operator*() const         { return { a->data_id_map[pos], a->data[pos] }; }
        const_reference operator[](ui32 p) const  { return *((*this) + p); }
    
    private:
    
        ui32 move(ui32 ofst)    { return pos = offset(ofst); }
        ui32 offset(ui32 ofst)  { return pos + ofst; }
    
        const this_set_t* a;
        ui32 pos;
    };
    
    template<bool is_forward = true>
    class Iterator_t { 
    public:
        using difference_type = std::ptrdiff_t;
        using reference = std::pair<Idx, C&>;
        using const_reference = std::pair<Idx, const C&>;
        using iterator_category = std::random_access_iterator_tag;
    
        using this_t = Iterator_t<is_forward>;
    
        Iterator_t(this_set_t& a, ui32 pos) : a(&a), pos(pos) {}
        Iterator_t() : a(nullptr), pos(0) {}
        Iterator_t(const this_t& it) : a(it.a), pos(it.pos) {}
        ~Iterator_t() = default;
    
        template<bool is_forw>
        operator ConstIterator_t<is_forw> () const { return a == nullptr ? ConstIterator_t<is_forw>() : ConstIterator_t<is_forw>(*a, pos); }
    
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
    
        const reference operator*() const         { return { a->data_id_map[pos], a->data[pos] }; }
        const reference operator[](ui32 p) const  { return *((*this) + p); }
    
        reference operator*()                     { return { a->data_id_map[pos], a->data[pos] }; }
        reference operator[](ui32 p)              { return *((*this) + p); }
    
    private:
    
        ui32 move(ui32 ofst)    { return pos = offset(ofst); }
        ui32 offset(ui32 ofst)  { return pos + ofst; }
    
        this_set_t* a;
        ui32 pos;
    };    
    
    using container_t = UninitializedArray<C, Size, grow_policy>;
    using forward_iterator = Iterator_t<true>;
    using backward_iterator = Iterator_t<false>;
    using forward_const_iterator = ConstIterator_t<true>;
    using backward_const_iterator = ConstIterator_t<false>;
    
    SparseSet() {
        std::fill(id_data_map, id_data_map + Size, -1);
    }

    ~SparseSet() {
        reset();
    }

    void reset() {
        for(int i = 0; i < used; ++i)
            data[i].~C();
        std::fill(id_data_map, id_data_map + Size, -1);
    }

    bool has(Idx id) {
        return id_data_map[id] != -1;
    }

    C& get(Idx id) {
        assert(has(id));
        return data[id_data_map[id]];
    }

    const C& get(Idx id) const {
        assert(has(id));
        return data[id_data_map[id]];
    }

    template<typename...Args>
    void add(Idx id, Args&&... args) {
        assert(!has(id));
        id_data_map[id] = used;
        data_id_map[used] = id;
        new (&data[used++]) C(std::forward<Args>(args)...);
    }

    void remove(Idx cur_id) {
        assert(has(cur_id));
        i32 cur_pos = id_data_map[cur_id];
        data[cur_pos].~C();

        i32 last_pos = used - 1;
        Idx last_id = data_id_map[last_pos];

        if (last_pos == cur_pos) {
            // cur is the last
            id_data_map[cur_id] = -1;
        } else {
            data[cur_pos] = std::move(data[last_pos]);
            data[last_pos].~C();
            data_id_map[cur_pos] = last_id;
            id_data_map[cur_id] = -1;
            id_data_map[last_id] = cur_pos;
        }
        --used;
    }

    forward_iterator begin()  { return forward_iterator(*this, 0); }
    forward_iterator end()    { return forward_iterator(*this, used); }
    backward_iterator rbegin(){ return backward_iterator(*this, 0); }
    backward_iterator rend()  { return backward_iterator(*this, used); }

    forward_const_iterator cbegin() const   { return forward_const_iterator(*this, 0); }
    forward_const_iterator cend() const     { return forward_const_iterator(*this, used); }
    backward_const_iterator crbegin() const { return backward_const_iterator(*this, 0); }
    backward_const_iterator crend() const   { return backward_const_iterator(*this, used); }

    ui32 size () const { return used; }

private:

    container_t data;
    Idx data_id_map[Size];
    i32 id_data_map[Size];
    i32 used = 0;

};

ECS_END_NS