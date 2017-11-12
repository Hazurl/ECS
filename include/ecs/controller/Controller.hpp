#pragma once

#include <ecs/Config.hpp>
#include <ecs/context/Systems.hpp>

ECS_BEGIN_NS

template<typename SystemsUpdater_t>
class Controller {
public:
    template<typename SC>
    Controller(SC const& sc = SystemsConstructor<>{}) : updater(controller, sc) {}

    Entity_t create () { 
        return controller.create();
    }

    void destroy (Entity_t ent) { 
        return controller.destroy(ent);
    }

    bool alive (Entity_t ent) const { 
        return controller.alive(ent);
    }

    ui32 size () const { 
        return controller.size(ent);
    }

    bool empty () const { 
        return controller.empty(ent);
    }

    template<typename...Comps>
    void remove_components(Entity_t ent) {
        return controller.remove_components<Comps...>(ent);
    }

    template<typename C>
    bool has_component(Entity_t ent) {
        return controller.has_component<C>(ent);
    }

    template<typename C>
    C& get_component(Entity_t ent) {
        return controller.get_component<C>(ent);
    }

    template<typename C>
    const C& get_component(Entity_t ent) const {
        return controller.get_component<C>(ent);
    }

    template<typename C, typename...Args>
    C& add_component(Entity_t ent, Args&&... args) {
        return controller.add_component<C>(ent, std::forward<Args>(args)...);
    }

    template<typename C, typename...Args>
    C& ensure_component(Entity_t ent, Args&&... args) {
        return controller.ensure_component<C>(ent, std::forward<Args>(args)...);
    }

    template<typename...Args>
    void update(Args&&...args) {
        updater.update(std::forward<Args>(args)...);
    }

private:

    typename SystemsUpdater_t::EntityController_t controller;
    SystemsUpdater_t updater;
    
};

ECS_END_NS