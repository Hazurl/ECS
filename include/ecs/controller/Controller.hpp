#pragma once

#include <ecs/Config.hpp>
#include <ecs/context/Systems.hpp>

ECS_BEGIN_NS

template<typename SystemsUpdater_t>
class Controller {
public:
    using Entity_t = typename SystemsUpdater_t::EntityController_t::Entity_t;

    template<typename SC>
    Controller(SC const& sc = SystemsConstructor<>{}) : updater(entity_controller, tag_controller, sc) {}

    Entity_t create () { 
        return entity_controller.create();
    }

    void destroy (Entity_t ent) { 
        return entity_controller.destroy(ent);
    }

    bool alive (Entity_t ent) const { 
        return entity_controller.alive(ent);
    }

    ui32 size () const { 
        return entity_controller.size();
    }

    bool empty () const { 
        return entity_controller.empty();
    }

    template<typename...Comps>
    void remove_components(Entity_t ent) {
        return entity_controller.template remove_components<Comps...>(ent);
    }

    template<typename C>
    bool has_component(Entity_t ent) {
        return entity_controller.template has_component<C>(ent);
    }

    template<typename C>
    C& get_component(Entity_t ent) {
        return entity_controller.template get_component<C>(ent);
    }

    template<typename C>
    const C& get_component(Entity_t ent) const {
        return entity_controller.template get_component<C>(ent);
    }

    template<typename C, typename...Args>
    C& add_component(Entity_t ent, Args&&... args) {
        return entity_controller.template add_component<C>(ent, std::forward<Args>(args)...);
    }

    template<typename C, typename...Args>
    C& ensure_component(Entity_t ent, Args&&... args) {
        return entity_controller.template ensure_component<C>(ent, std::forward<Args>(args)...);
    }

    template<typename...Args>
    void update(Args&&...args) {
        updater.update(std::forward<Args>(args)...);
    }

    template<typename T>
    bool has_tag(Entity_t ent) const {
        return tag_controller.template has<T>(ent.id());
    }

    template<typename T>
    void set_tag(Entity_t ent) {
        return tag_controller.template set<T>(ent.id());
    }

    template<typename T>
    void remove_tag(Entity_t ent) {
        return tag_controller.template remove<T>(ent.id());
    }

    template<typename...Tags>
    void reset_tags(Entity_t ent) {
        return tag_controller.template remove<Tags...>(ent.id());
    }

private:

    typename SystemsUpdater_t::EntityController_t entity_controller;
    typename SystemsUpdater_t::TagController_t tag_controller;
    SystemsUpdater_t updater;
    
};

ECS_END_NS