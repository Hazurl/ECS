#pragma once

#include <ecs/Config.hpp>
#include <ecs/policy/grow_policy.hpp>
#include <ecs/container/RawArray.hpp>
#include <ecs/container/iterators/Iterator.hpp>

#include <mtp/Utils.hpp>

#include <cmath>

ECS_BEGIN_NS

template<typename C, ui32 size, typename grow_policy, bool _ = (std::is_same<typename grow_policy::tag, tag_grow_policy>::value) >
class UninitializedArray {
    static_assert(!_, "Unknown grow policy");
};

template<typename C, ui32 size, typename grow_policy>
class UninitializedArray<C, size, grow_policy, false> {
    static_assert(mtp::AlwaysFalse<C>::value, "policy must define tag 'tag_grow_policy'");
};

template<typename C, ui32 Size>
class UninitializedArray<C, Size, instant_grow_policy, true> {
public:
    using forward_iterator = Iterator<UninitializedArray<C, Size, instant_grow_policy, true>, C, true>;
    using backward_iterator = Iterator<UninitializedArray<C, Size, instant_grow_policy, true>, C, false>;
    using forward_const_iterator = ConstIterator<UninitializedArray<C, Size, instant_grow_policy, true>, C, true>;
    using backward_const_iterator = ConstIterator<UninitializedArray<C, Size, instant_grow_policy, true>, C, false>;
    
    C& operator [] (i32 id) {
        return data[id].cast();
    }

    const C& operator [] (i32 id) const {
        return data[id].cast();
    }

    forward_iterator begin()  { return forward_iterator(*this, 0); }
    forward_iterator end()    { return forward_iterator(*this, Size); }
    backward_iterator rbegin(){ return backward_iterator(*this, 0); }
    backward_iterator rend()  { return backward_iterator(*this, Size); }

    forward_const_iterator cbegin() const   { return forward_const_iterator(*this, 0); }
    forward_const_iterator cend() const     { return forward_const_iterator(*this, Size); }
    backward_const_iterator crbegin() const { return backward_const_iterator(*this, 0); }
    backward_const_iterator crend() const   { return backward_const_iterator(*this, Size); }

    constexpr ui32 size() { return Size; }
    
private:

    SizedRawArray<C, Size> data;
};

template<typename C, ui32 max_size, i32 InitialSize, i32 Factor>
class UninitializedArray<C, max_size, growing_grow_policy<InitialSize, Factor>, true> {
public:
    using this_type = UninitializedArray<C, max_size, growing_grow_policy<InitialSize, Factor>, true>;

    using forward_iterator = Iterator<this_type, C, true>;
    using backward_iterator = Iterator<this_type, C, false>;
    using forward_const_iterator = ConstIterator<this_type, C, true>;
    using backward_const_iterator = ConstIterator<this_type, C, false>;

    UninitializedArray() : data(InitialSize), _size(InitialSize) {}
    ~UninitializedArray() {}

    UninitializedArray(this_type const& o) = delete;

    UninitializedArray(this_type&& o) : data(std::move(o.data)), _size(o._size) {}

    this_type& operator=(this_type&& o) {
        data = std::move(o.data);
        _size = o._size;
        return *this;
    }

    C& operator [] (ui32 id) {
        if (id >= _size)
            resize(id);
        return data[id].cast();
    }

    const C& operator [] (ui32 id) const {
        return data[id].cast();
    }

    void resize(ui32 atleast) {
        ui32 new_size = _size * Factor;
        while(new_size < atleast && new_size < max_size) new_size *= Factor;
        if (new_size > max_size) new_size = max_size;

        if (_size != new_size) {
            RawArray<C> new_data(new_size);
            new_data.copy_raw(data, _size);
            data = std::move(new_data);
            _size = new_size;
        }
    }

    forward_iterator begin()  { return forward_iterator(*this, 0); }
    forward_iterator end()    { return forward_iterator(*this, _size); }
    backward_iterator rbegin(){ return backward_iterator(*this, 0); }
    backward_iterator rend()  { return backward_iterator(*this, _size); }

    forward_const_iterator cbegin() const   { return forward_const_iterator(*this, 0); }
    forward_const_iterator cend() const     { return forward_const_iterator(*this, _size); }
    backward_const_iterator crbegin() const { return backward_const_iterator(*this, 0); }
    backward_const_iterator crend() const   { return backward_const_iterator(*this, _size); }

    ui32 size() { return _size; }

private:

    RawArray<C> data;
    ui32 _size = InitialSize;
};

template<typename C, ui32 max_size, i32 N>
class UninitializedArray<C, max_size, bucket_grow_policy<N>, true> {
public:
    using this_type = UninitializedArray<C, max_size, bucket_grow_policy<N>, true>;
    
    using forward_iterator = Iterator<this_type, C, true>;
    using backward_iterator = Iterator<this_type, C, false>;
    using forward_const_iterator = ConstIterator<this_type, C, true>;
    using backward_const_iterator = ConstIterator<this_type, C, false>;

    UninitializedArray() {
        std::fill(data, data + count_bucket, nullptr);        
    }

    ~UninitializedArray() { 
        for(int i = 0; i < count_bucket; ++i)
            if (data[i] != nullptr)
                delete data[i];
    }

    UninitializedArray(this_type const& o) = delete;

    UninitializedArray(this_type&& o) {
        for(int i = 0; i < count_bucket; ++i)
            data[i] = (o.data[i] == nullptr) ? 
                nullptr : 
                new SizedRawArray<C, N>(std::move(o.data[i]));
    }

    this_type& operator=(this_type&& o) {
        for(int i = 0; i < count_bucket; ++i)
            data[i] = (o.data[i] == nullptr) ? 
                nullptr : 
                new SizedRawArray<C, N>(std::move(o.data[i]));
        return *this;
    }

    C& operator [] (i32 id) {
        return get_bucket(id)[id].cast();
    }

    const C& operator [] (i32 id) const {
        return data[get_bucket_idx(id)][id].cast();
    }

    forward_iterator begin()  { return forward_iterator(*this, 0); }
    forward_iterator end()    { return forward_iterator(*this, max_size); }
    backward_iterator rbegin(){ return backward_iterator(*this, 0); }
    backward_iterator rend()  { return backward_iterator(*this, max_size); }

    forward_const_iterator cbegin() const   { return forward_const_iterator(*this, 0); }
    forward_const_iterator cend() const     { return forward_const_iterator(*this, max_size); }
    backward_const_iterator crbegin() const { return backward_const_iterator(*this, 0); }
    backward_const_iterator crend() const   { return backward_const_iterator(*this, max_size); }

    constexpr ui32 size() { return max_size; }

private:
    i32 get_bucket_idx (i32 id) const {
        return id / N;
    }

    SizedRawArray<C, N>& get_bucket(i32 id) {
        i32 bucket_idx = get_bucket_idx(id);
        auto& b = data[bucket_idx];
        if (b == nullptr)
            b = new SizedRawArray<C, N>();
        return *b;
    }

    static constexpr i32 count_bucket = (max_size + 1) / N;
    SizedRawArray<C, N>* data[count_bucket];
};

ECS_END_NS