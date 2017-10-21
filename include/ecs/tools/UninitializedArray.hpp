#pragma once

#include <ecs/Config.hpp>
#include <ecs/policy/grow_policy.hpp>
#include <ecs/tools/RawArray.hpp>

#include <mtp/Utils.h>

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

template<typename C, ui32 size>
class UninitializedArray<C, size, instant_grow_policy, true> {
public:
    C& operator [] (i32 id) {
        return data[id].cast();
    }

    const C& operator [] (i32 id) const {
        return data[id].cast();
    }

private:

    SizedRawArray<C, size> data;
};

template<typename C, ui32 max_size, i32 InitialSize, i32 Factor>
class UninitializedArray<C, max_size, growing_grow_policy<InitialSize, Factor>, true> {
public:
    using this_type = UninitializedArray<C, max_size, growing_grow_policy<InitialSize, Factor>, true>;
    
    UninitializedArray() : data(InitialSize), size(InitialSize) {}
    ~UninitializedArray() {}

    UninitializedArray(this_type const& o) = delete;

    UninitializedArray(this_type&& o) : data(std::move(o.data)), size(o.size) {}

    this_type& operator=(this_type&& o) {
        data = std::move(o.data);
        size = o.size;
        return *this;
    }

    C& operator [] (ui32 id) {
        if (id >= size)
            resize(id);
        return data[id].cast();
    }

    const C& operator [] (ui32 id) const {
        return data[id].cast();
    }

    void resize(ui32 atleast) {
        ui32 new_size = size * Factor;
        while(new_size < atleast && new_size < max_size) new_size *= Factor;
        if (new_size > max_size) new_size = max_size;

        if (size != new_size) {
            RawArray<C> new_data(new_size);
            new_data.copy_raw(data, size);
            data = std::move(new_data);
            size = new_size;
        }
    }

private:

    RawArray<C> data;
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