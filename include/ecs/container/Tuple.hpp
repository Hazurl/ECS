#pragma once

#include <ecs/Config.hpp>
#include <ecs/container/RawArray.hpp>

#include <mtp/Utils.hpp>
#include <mtp/list/List.hpp>

#include <cassert>
#include <utility>

ECS_BEGIN_NS

template<i32 I, typename T>
class Tuple_singleton : public mtp::TConst<T> {
public:

    ~Tuple_singleton() {
        if (has_been_contructed)
            value.cast().~T();
    }

    RawData<T> value;

protected:

    template<typename...Args>
    void construct_it (Args&&...args) {
        assert(!has_been_contructed);
        ::new (&value.cast()) T (std::forward<Args>(args)...);
        has_been_contructed = true;
    }

    bool has_been_contructed = false;

}; 

template<i32 I, typename...Args>
class Tuple_helper {};

template<i32 I, typename T, typename...Args>
class Tuple_helper<I, T, Args...> : public Tuple_singleton<I, T>, public Tuple_helper<I+1, Args...> {};

template<typename...Args>
class Tuple : private Tuple_helper<0, Args...> {
public:

    template<typename T, typename...Brgs>
    std::enable_if_t<mtp::index_of_v<mtp::List<Args...>, T> != -1, 
    T& > construct (Brgs&&...brgs) {
        Tuple_singleton<mtp::index_of_v<mtp::List<Args...>, T>, T>::construct_it(std::forward<Brgs>(brgs)...);
        return get<T>();
    }

    template<typename T>
    std::enable_if_t<mtp::index_of_v<mtp::List<Args...>, T> != -1, 
    T& > get () {
        return Tuple_singleton<mtp::index_of_v<mtp::List<Args...>, T>, T>::value.cast();
    }

};

template<typename...Args>
class Tuple<mtp::List<Args...>> : public Tuple<Args...> {};

ECS_END_NS