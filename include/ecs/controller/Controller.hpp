#pragma once

#include <ecs/Config.hpp>
#include <ecs/context/Systems.hpp>

ECS_BEGIN_NS

template<typename SystemsUpdater_t>
class Controller {
public:
    template<typename SC>
    Controller(SC const& sc = SystemsConstructor<>{}) : updater(controller, sc) {}

    template<typename...Args>
    void update(Args&&...args) {
        updater.update(std::forward<Args>(args)...);
    }

private:

    typename SystemsUpdater_t::EntityController_t controller;
    SystemsUpdater_t updater;
    
};

ECS_END_NS