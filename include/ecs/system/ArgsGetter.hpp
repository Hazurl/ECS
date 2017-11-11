#pragma once

#include <ecs/Config.hpp>

#include <mtp/list/List.hpp>

ECS_BEGIN_NS

template<typename L, typename T, i32 count>
struct InsertIfNotInHelper : public mtp::TConst<L> {};

template<typename L, typename T>
struct InsertIfNotInHelper<L, T, 0> : public mtp::TConst<mtp::insert_back<L, T>> {};

template<typename L, typename T>
struct InsertIfNotIn : public InsertIfNotInHelper<L, T, mtp::count_v<L, T>> {};

template<typename L0, typename L1>
struct MergeUniqueList {
    static_assert(mtp::AlwaysFalse<L0>::value, "!");
};

template<typename...Args, typename B, typename...Brgs>
struct MergeUniqueList<mtp::List<Args...>, mtp::List<B, Brgs...>> : public MergeUniqueList<mtp::type_of<InsertIfNotIn<mtp::List<Args...>, B>>, mtp::List<Brgs...>> {};

template<typename...Args>
struct MergeUniqueList<mtp::List<Args...>, mtp::List<>> : public mtp::TConst<mtp::List<Args...>> {};

template<typename F>
struct ListOfArgs{
    static_assert(mtp::AlwaysFalse<F>::value, "!");
};

template<typename R, typename C, typename...Args>
struct ListOfArgs<R(C::*)(Args...)> : public mtp::TConst<mtp::List<Args...>> {};

template<typename L, typename R>
class ArgsGetterHelper {
    static_assert(mtp::AlwaysFalse<L>::value, "!");
};

template<typename A, typename...Args, typename R>
class ArgsGetterHelper<mtp::List<A, Args...>, R> : public 
    MergeUniqueList<
        mtp::type_of<
            ArgsGetterHelper<
                mtp::List<Args...>, 
                R
            >
        >, 
        mtp::type_of<
            ListOfArgs< 
                typename A::type 
            >
        >
    > {
};

template<typename R>
class ArgsGetterHelper<mtp::List<>, R> : public mtp::TConst<R> {};

template<typename Methods>
using ArgsGetter = mtp::type_of<ArgsGetterHelper<Methods, mtp::List<>>>;

ECS_END_NS