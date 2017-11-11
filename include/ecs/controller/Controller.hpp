#pragma once

#include <ecs/Config.hpp>
#include <ecs/container/Tuple.hpp>

#include <ecs/entity/EntityManager.hpp>
#include <ecs/entity/EntityController.hpp>

#include <ecs/context/Context.hpp>

#include <ecs/view/Views.hpp>

#include <ecs/component/ComponentPool.hpp>

#include <mtp/Utils.hpp>
#include <mtp/list/List.hpp>

#include <tuple>

ECS_BEGIN_NS

template<typename SystemsUpdater_t, typename EntityController_t>
class Controller {

    template<typename SC>
    Controller(SC sc) : updater(controller, sc) {}
    
    EntityController_t controller;
    SystemsUpdater_t updater;
    
};

ECS_END_NS