#pragma once

#include <ecs/Config.hpp>

#include <unordered_map>

ECS_BEGIN_NS

template<typename L, typename R>
class BiMap {
public:

    void insert(L const& l, R const& r) {
        erase_left(l);
        erase_right(r);

        auto const& direct_inserted = direct.insert({l, nullptr}).first;
        auto const& reverse_inserted =  reverse.insert({r, nullptr}).first;

        direct_inserted->second = &reverse_inserted->first;
        reverse_inserted->second = &direct_inserted->first;
    }

    void erase_left(L const& l) {
        auto it = direct.find(l);
        if (it != direct.end()) {
            reverse.erase(*it->second);
            direct.erase(it);
        }
    }

    void erase_right(R const& r) {
        auto it = reverse.find(r);
        if (it != reverse.end()) {
            direct.erase(*it->second);
            reverse.erase(it);
        }
    }

    const R* find_right(L const& l) const {
        auto it = direct.find(l);
        return it == direct.end() ? nullptr : it->second;
    }

    const L* find_left(R const& r) const {
        auto it = reverse.find(r);
        return it == reverse.end() ? nullptr : it->second;
    }

    template<typename F>
    void for_each(F f) {
        for(auto& p : direct)
            f(p.first, *p.second);
    }

    template<typename F>
    void for_each(F f) const {
        for(auto const& p : direct)
            f(p.first, *p.second);
    }

    bool empty() const {
        return direct.empty();
    }

    std::size_t size() const {
        return direct.size();
    }

    void clear() const {
        direct.clear();
        reverse.clear();
    }

    void reserve(std::size_t size) {
        direct.reserve(size);
        reverse.reserve(size);
    }

private:

    std::unordered_map<L, const R*> direct;
    std::unordered_map<R, const L*> reverse;
    
};

ECS_END_NS