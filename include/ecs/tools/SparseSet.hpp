#pragma once

#include <ecs/Config.hpp>
#include <ecs/tools/RawArray.hpp>

#include <array>
#include <cassert>

ECS_BEGIN_NS

template<typename C, ui32 size>
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
            raw_cast<C>(data[i]).~C();
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
        raw_cast<C>(data[cidx]).~C();
        
        if (used > 1) { // move last
            i32 last = data_id_map[used - 1];
            if (last != id) {
                raw_cast<C>(data[cidx]) = std::move(raw_cast<C>(data[used - 1]));
                raw_cast<C>(data[used - 1]).~C();

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
            f(raw_cast<C>(data[i]));
    }

    template<typename F>
    void for_each(F f) const {
        for (int i = 0; i < used; ++i)
            f(raw_cast<C>(data[i]));
    }

private:

    RawArray<C, size> data;
    i32 data_id_map[size];
    i32 id_data_map[size];
    i32 used = 0;

};

ECS_END_NS