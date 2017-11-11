#pragma once

#include <ecs/Config.hpp>

ECS_BEGIN_NS

template<typename SystemsUpdater_t>
class Controller {

    template<typename SC>
    Controller(SC sc) : updater(controller, sc) {}

    template<typename...Args>
    void update(Args&&...args) {
        updater.update(std::forward<Args>(args)...);
    }

//private:

    typename SystemsUpdater_t::EntityController_t controller;
    SystemsUpdater_t updater;
    
};

ECS_END_NS