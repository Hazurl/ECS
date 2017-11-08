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
            destroy_it();
    }

    RawData<T> value;

protected:

    template<typename...Args>
    void construct_it (Args&&...args) {
        assert(!has_been_contructed);
        value.construct(std::forward<Args>(args)...);
        has_been_contructed = true;
    }

    template<typename...Args>
    void destroy_it (Args&&...args) {
        assert(has_been_contructed);
        value.destroy();
        has_been_contructed = false;
    }

    template<typename...Args>
    void reconstruct_it (Args&&...args) {
        if(has_been_contructed)
            destroy_it();
        construct_it(std::forward<Args>(args)...);
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
    std::enable_if_t<mtp::in_v<mtp::List<Args...>, T>, 
    T& > construct (Brgs&&...brgs) {
        Tuple_singleton<mtp::index_of_v<mtp::List<Args...>, T>, T>::construct_it(std::forward<Brgs>(brgs)...);
        return get<T>();
    }

    template<typename T, typename...Brgs>
    std::enable_if_t<mtp::in_v<mtp::List<Args...>, T>, 
    T& > reconstruct (Brgs&&...brgs) {
        Tuple_singleton<mtp::index_of_v<mtp::List<Args...>, T>, T>::reconstruct_it(std::forward<Brgs>(brgs)...);
        return get<T>();
    }

    template<typename T>
    std::enable_if_t<mtp::in_v<mtp::List<Args...>, T>, 
    T& > get () {
        return Tuple_singleton<mtp::index_of_v<mtp::List<Args...>, T>, T>::value.cast();
    }

};

template<typename L>
class Tuple_from_list_helper : public mtp::TConst<Tuple<>> {
    static_assert(mtp::AlwaysFalse<L>::value, "Tuple_from_list want a List in argument");
};

template<typename...Args>
class Tuple_from_list_helper<mtp::List<Args...>> : public mtp::TConst<Tuple<Args...>> {};

template<typename L>
using Tuple_from_list = mtp::type_of<Tuple_from_list_helper<L>>;

ECS_END_NS