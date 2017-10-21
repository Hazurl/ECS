#pragma once

#include <ecs/Config.hpp>
#include <ecs/policy/grow_policy.hpp>
#include <ecs/tools/RawArray.hpp>

#include <cmath>

ECS_BEGIN_NS

template<typename C, ui32 size, typename grow_policy, bool _ = (std::is_same<typename grow_policy::tag, tag_grow_policy>::value) >
class UninitializedArray {
    static_assert(!_, "Unknown grow policy");
};

template<typename C, ui32 size, typename grow_policy>
class UninitializedArray<C, size, grow_policy, false> {
    static_assert(std::is_same<typename grow_policy::tag, tag_grow_policy>::value, "policy must define tag 'tag_grow_policy'");
};

template<typename C, ui32 size>
class UninitializedArray<C, size, instant_grow_policy, true> {
public:
    C& operator [] (i32 id) {
        return raw_cast<C>(data[id]);
    }

    const C& operator [] (i32 id) const {
        return raw_cast<C>(data[id]);
    }

private:

    RawArray<C, size> data;
};

template<typename C, ui32 max_size, i32 InitialSize, i32 Factor>
class UninitializedArray<C, max_size, growing_grow_policy<InitialSize, Factor>, true> {
public:
    using this_type = UninitializedArray<C, max_size, growing_grow_policy<InitialSize, Factor>, true>;
    
    UninitializedArray() : data(new_RawArray<C>(InitialSize)) {}
    ~UninitializedArray() { delete_RawArray<C>(data); }

    UninitializedArray(this_type const& o) {
        data = new_RawArray<C>(o.size);
        size = o.size;
        copy_RawArray<C>(o.data, data, o.size);
    }

    UninitializedArray(this_type&& o) {
        data = new_RawArray<C>(o.size);
        size = o.size;
        move_RawArray<C>(o.data, data, o.size);
    }

    this_type& operator=(this_type o) {
        std::swap(data, o.data);
        return *this;
    }

    C& operator [] (i32 id) {
        if (id >= static_cast<i32>(size))
            resize(id);
        return raw_cast<C>(data[id]);
    }

    const C& operator [] (i32 id) const {
        return raw_cast<C>(data[id]);
    }

    void resize(i32 atleast) {
        if (size < max_size) {
            i32 new_size = size * Factor;
            while(new_size < static_cast<i32>(max_size) && new_size < atleast) new_size *= Factor;

            RawArrayPtr<C> new_data = new_RawArray<C>(new_size);
            move_RawArray<C>(data, new_data, size);
            delete_RawArray<C>(data);

            data = new_data;
            size = new_size;
        }
    }

private:

    RawArrayPtr<C> data;
    ui32 size = InitialSize;
};

template<typename C, ui32 max_size, i32 N>
class UninitializedArray<C, max_size, bucket_grow_policy<N>, true> {
public:
    using this_type = UninitializedArray<C, max_size, bucket_grow_policy<N>, true>;
    
    UninitializedArray() {
        std::fill(data, data + count_bucket, nullptr);        
    }

    ~UninitializedArray() { 
        for(int i = 0; i < count_bucket; ++i)
            if (data[i] != nullptr)
                delete_RawArray<C>(data[i]);
    }

    UninitializedArray(this_type const& o) {
        for(int i = 0; i < count_bucket; ++i) {
            if (o.data[i] != nullptr) {
                data[i] = new_RawArray<C>(N);
                copy_RawArray<C>(o.data[i], data[i], N);
            } else {
                data[i] = nullptr;
            }
        }
    }

    UninitializedArray(this_type&& o) {
        for(int i = 0; i < count_bucket; ++i) {
            if (o.data[i] != nullptr) {
                data[i] = new_RawArray<C>(N);
                move_RawArray<C>(o.data[i], data[i], N);
            } else {
                data[i] = nullptr;
            }
        }
    }

    this_type& operator=(this_type o) {
        std::swap(data, o.data);
        return *this;
    }

    C& operator [] (i32 id) {
        return raw_cast<C>(get_bucket(id)[id]);
    }

    const C& operator [] (i32 id) const {
        return raw_cast<C>(data[get_bucket_idx(id)][id]);
    }

private:
    static constexpr i32 count_bucket = std::ceil(static_cast<float>(max_size) / N);

    i32 get_bucket_idx (i32 id) const {
        return std::floor(static_cast<float>(id) / N);
    }

    RawArrayPtr<C> get_bucket(i32 id) {
        i32 bucket_idx = get_bucket_idx(id);
        RawArrayPtr<C>& b = data[bucket_idx];
        if (b == nullptr)
            b = new_RawArray<C>(N);
        return b;
    }

    RawArrayPtr<C> data[count_bucket];
};

ECS_END_NS