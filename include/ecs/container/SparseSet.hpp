#pragma once

#include <ecs/Config.hpp>
#include <ecs/container/UninitializedArray.hpp>

#include <cassert>

ECS_BEGIN_NS

template<typename C, ui32 size, typename Idx, typename grow_policy = instant_grow_policy>
class SparseSet {
public:

    SparseSet() {
        std::fill(id_data_map, id_data_map + size, -1);
    }

    ~SparseSet() {
        reset();
    }

    void reset() {
        for(int i = 0; i < used; ++i)
            data[i].~C();
        std::fill(id_data_map, id_data_map + size, -1);
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

    UninitializedArray<C, size, grow_policy> data;
    Idx data_id_map[size];
    i32 id_data_map[size];
    i32 used = 0;

};

ECS_END_NS