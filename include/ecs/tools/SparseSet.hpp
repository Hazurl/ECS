#pragma once

#include <ecs/Config.hpp>
#include <ecs/tools/RawArray.hpp>
#include <ecs/policy/grow_policy.hpp>

#include <vector>
#include <cassert>
#include <cmath>

ECS_BEGIN_NS

template<typename C, ui32 size, typename grow_policy, bool _ = (std::is_same<typename grow_policy::tag, tag_grow_policy>::value) >
struct SparseSetData {
    static_assert(_, "Unknown grow policy");
};

template<typename C, ui32 size, bool _>
struct SparseSetData<C, size, instant_grow_policy, _> {
    C& operator [] (i32 id) {
        return raw_cast<C>(data[id]);
    }

    const C& operator [] (i32 id) const {
        return raw_cast<C>(data[id]);
    }

    RawArray<C, size> data;
};

template<typename C, ui32 max_size, bool _, i32 InitialSize, i32 Factor>
struct SparseSetData<C, max_size, growing_grow_policy<InitialSize, Factor>, _> {
    SparseSetData() : data(new_RawArray<C>(InitialSize)) {}
    ~SparseSetData() { delete_RawArray<C>(data); }
    
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

    RawArrayPtr<C> data;
    ui32 size = InitialSize;
};

template<typename C, ui32 max_size, bool _, i32 N>
struct SparseSetData<C, max_size, bucket_grow_policy<N>, _> {
    static constexpr i32 count_bucket = std::ceil(static_cast<float>(max_size) / N);
    
    SparseSetData() {
        std::fill(data, data + count_bucket, nullptr);        
    }
    ~SparseSetData() { 
        for(int i = 0; i < count_bucket; ++i)
            if (data[i] != nullptr)
                delete data[i];
    }

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
    
    C& operator [] (i32 id) {
        return raw_cast<C>(get_bucket(id)[id]);
    }

    const C& operator [] (i32 id) const {
        return raw_cast<C>(data[get_bucket_idx(id)][id]);
    }

    RawArrayPtr<C> data[count_bucket];
};

template<typename C, ui32 size, typename grow_policy = instant_grow_policy>
class SparseSet {
public:

    SparseSet() {
        std::fill(data_id_map, data_id_map + size, -1);
        std::fill(id_data_map, id_data_map + size, -1);
    }

    ~SparseSet() {
        reset();
    }

    void reset() {
        for(int i = 0; i < used; ++i)
            data[i].~C();
        std::fill(data_id_map, data_id_map + size, -1);
        std::fill(id_data_map, id_data_map + size, -1);
    }

    bool has(i32 id) {
        assert(id >= 0);
        return id_data_map[id] != -1;
    }

    C& get(i32 id) {
        assert(has(id));
        return data[id_data_map[id]];
    }

    const C& get(i32 id) const {
        assert(has(id));
        return data[id_data_map[id]];
    }

    template<typename...Args>
    void add(i32 id, Args&&... args) {
        assert(!has(id));
        id_data_map[id] = used;
        data_id_map[used] = id;
        new (&data[used++]) C(std::forward<Args>(args)...);
    }

    void remove(i32 id) {
        assert(has(id));
        i32 cidx = id_data_map[id];
        data[cidx].~C();
        
        if (used > 1) { // move last
            i32 last = data_id_map[used - 1];
            if (last != id) {
                data[cidx] = std::move(data[used - 1]);
                data[used - 1].~C();

                id_data_map[id] = cidx;
                data_id_map[cidx] = last;
                id_data_map[last] = -1;
                data_id_map[used - 1] = -1;
            }
        } else {
            id_data_map[id] = data_id_map[cidx] = -1;            
        }

        --used;
    }

    template<typename F>
    void for_each(F f) {
        for (int i = 0; i < used; ++i)
            f(data[i]);
    }

    template<typename F>
    void for_each(F f) const {
        for (int i = 0; i < used; ++i)
            f(data[i]);
    }

private:

    SparseSetData<C, size, grow_policy> data;
    i32 data_id_map[size];
    i32 id_data_map[size];
    i32 used = 0;

};

ECS_END_NS