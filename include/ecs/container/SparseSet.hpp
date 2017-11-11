#pragma once

#include <ecs/Config.hpp>
#include <ecs/container/UninitializedArray.hpp>

#include <cassert>

ECS_BEGIN_NS

template<typename C, ui32 Size, typename Key, typename grow_policy = instant_grow_policy>
class SparseSet {
public:

    using this_t = SparseSet<C, Size, Key, grow_policy>;
    using container_t = UninitializedArray<C, Size, grow_policy>;
    using forward_iterator = RandomRefIterator<this_t, C, true>;
    using backward_iterator = RandomRefIterator<this_t, C, false>;
    using forward_const_iterator = ConstRandomIterator<this_t, C, true>;
    using backward_const_iterator = ConstRandomIterator<this_t, C, false>;

    struct Keys {

        using forward_const_iterator = ConstRandomIterator<Keys, Key, true>;
        using backward_const_iterator = ConstRandomIterator<Keys, Key, false>;

        forward_const_iterator cbegin() const   { return forward_const_iterator(*this, 0); }
        forward_const_iterator cend() const     { return forward_const_iterator(*this, size()); }
        backward_const_iterator crbegin() const { return backward_const_iterator(*this, 0); }
        backward_const_iterator crend() const   { return backward_const_iterator(*this, size()); }

        forward_const_iterator begin() const   { return forward_const_iterator(*this, 0); }
        forward_const_iterator end() const     { return forward_const_iterator(*this, size()); }
        backward_const_iterator rbegin() const { return backward_const_iterator(*this, 0); }
        backward_const_iterator rend() const   { return backward_const_iterator(*this, size()); }
        
        ui32 size () const { return set->size(); }

        const Key& operator[] (ui32 pos) const { return set->data_to_key[pos]; } 

        ~Keys() = default;
        
    private:
        friend this_t;
        Keys(const this_t* set) : set(set) {}

        const this_t* set;
    };
    

    SparseSet() {
        std::fill(key_to_data, key_to_data + Size, -1);
    }

    ~SparseSet() {
        reset();
    }

    void reset() {
        for(int i = 0; i < used; ++i)
            datas.destroy(i);
        std::fill(key_to_data, key_to_data + Size, -1);
    }

    bool has(Key id) {
        return key_to_data[id] != -1;
    }

    C& get(Key id) {
        assert(has(id));
        return datas[key_to_data[id]];
    }

    const C& get(Key id) const {
        assert(has(id));
        return datas[key_to_data[id]];
    }

    template<typename...Args>
    void add(Key id, Args&&... args) {
        assert(!has(id));
        key_to_data[id] = used;
        data_to_key[used] = id;
        datas.construct(used++, std::forward<Args>(args)...);
    }

    void remove(Key cur_id) {
        assert(has(cur_id));
        i32 cur_pos = key_to_data[cur_id];
        datas.destroy(cur_pos);

        i32 last_pos = used - 1;
        Key last_id = data_to_key[last_pos];

        if (last_pos == cur_pos) {
            // cur is the last
            key_to_data[cur_id] = -1;
        } else {
            datas[cur_pos] = std::move(datas[last_pos]);
            datas.destroy(last_pos);
            data_to_key[cur_pos] = last_id;
            key_to_data[cur_id] = -1;
            key_to_data[last_id] = cur_pos;
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

    forward_const_iterator begin() const   { return cbegin(); }
    forward_const_iterator end() const     { return cend(); }
    backward_const_iterator rbegin() const { return crbegin(); }
    backward_const_iterator rend() const   { return crend(); }

    Keys keys() const { return Keys{this}; }

    ui32 size () const { return used; }

private:

    // Iterators
    friend forward_iterator;
    friend forward_const_iterator;
    friend backward_iterator;
    friend backward_const_iterator;
    C& operator [] (ui32 pos) { return datas[pos]; }
    const C& operator [] (ui32 pos) const { return datas[pos]; }
    
    container_t datas;
    Key data_to_key[Size];
    i32 key_to_data[Size];
    i32 used = 0;

};

ECS_END_NS