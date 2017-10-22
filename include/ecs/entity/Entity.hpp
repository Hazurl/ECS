#pragma once

#include <ecs/Config.hpp>
#include <ecs/policy/mask_policy.hpp>

ECS_BEGIN_NS

template<typename mask_policy>
struct Entity {
    using T = uint_of_size<mask_policy::total_size>;
    using T_id = uint_of_size<mask_policy::id_size>;
    using T_version = uint_of_size<mask_policy::version_size>;

    static Entity<mask_policy> create(T_id id, T_version version) {
        return { static_cast<T>(
            (static_cast<T_version>(version & mask_policy::version_mask) << mask_policy::version_shift) |
            (static_cast<T_id     >(id      & mask_policy::id_mask     ) << mask_policy::id_shift     )
        ) };
    }

    Entity(T ent) : entity(ent) {}

    inline T_id id() const {
        return static_cast<T_id     >((entity >> mask_policy::id_shift     ) & mask_policy::id_mask     );
    }

    inline T_version version() const {
        return static_cast<T_version>((entity >> mask_policy::version_shift) & mask_policy::version_mask);
    }

    const T entity;    
};

ECS_END_NS