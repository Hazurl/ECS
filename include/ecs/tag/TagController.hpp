#pragma once

#include <ecs/Config.hpp>

#include <bitset>
#include <vector>

ECS_BEGIN_NS

template<typename, typename, typename>
class SystemUpdater;

template<typename Tags, typename limit_size>
class TagController {
    using this_t = TagController<Tags, limit_size>;
public:

    class user_bridge {
    public:
        using Func = std::function<void(this_t&)>;
        user_bridge(this_t& controller) : controller(controller) {}

        template<typename T>
        bool has(i32 ent) const {
            return controller.has<T>(ent);
        }

        user_bridge& operator += (Func func) {
            funcs.push_back(func);
            return *this;
        }

    private:
        template<typename, typename, typename>
        friend class SystemUpdater;

        void execute() {
            for(auto& func : funcs)
                func(controller);
            funcs.clear();
        }

        std::vector<Func> funcs;
        this_t& controller;
    };

    template<typename T>
    bool has(i32 ent) const {
        return bits_tags[index_of<T>(ent)];
    }

    template<typename T>
    void set(i32 ent) {
        bits_tags[index_of<T>(ent)] = true;
    }

    template<typename T>
    void remove(i32 ent) {
        bits_tags[index_of<T>(ent)] = false;
    }

    template<typename...Ts>
    void reset(i32 ent) {
        mtp::apply_lambda<
            mtp::switch_t<
                mtp::empty_v<mtp::List<Ts...>>, // if Ts is empty, then reset all components
                Tags, 
                mtp::List<Ts...>
            >
        >{}([ent, this] (auto t) {
            this->remove<mtp::type_of<decltype(t)>>(ent);
        });
    }

private:

    static constexpr ui32 tags_size = mtp::size_v<Tags>;
    std::bitset<tags_size * limit_size::value> bits_tags;
    // bits_tags[entity * tags_size + tag_id]

    template<typename T>
    constexpr ui32 tag_id() const {
        return mtp::index_of_v<Tags, T>;
    }

    template<typename T>
    constexpr ui32 index_of(i32 ent) const {
        return ent * tags_size + tag_id<T>();
    }

};

ECS_END_NS