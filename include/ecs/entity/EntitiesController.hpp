#pragma once

#include <ecs/Config.hpp>

#include <functional>
#include <vector>

ECS_BEGIN_NS

template<typename Controller_t>
class EntitiesController {
public:
    using Func = std::function<void(Controller_t &)>;

    void alter_safely(Func f) {
        funcs.emplace_back(f);
    }

private:

    friend Controller_t;

    std::vector<Func> funcs;
    
};

ECS_END_NS