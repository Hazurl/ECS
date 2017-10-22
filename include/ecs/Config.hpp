#pragma once

#include<mtp/Utils.h>

#define ECS_NS_ECS ecs
#define ECS_BEGIN_NS namespace ecs {
#define ECS_END_NS }

ECS_BEGIN_NS

using i8 = signed char;
using ui8 = unsigned char;

using i16 = signed short;
using ui16 = unsigned short;

using i32 = signed int;
using ui32 = unsigned int;

using i64 = signed long;
using ui64 = unsigned long;

using f32 = float;
using f64 = double;
using f128 = long double;

namespace details {

template<ui8 S>
struct uint_of_size : public mtp::TConst<void> {
    static_assert(S, "not unsigned integer found for this size");
};

template<>
struct uint_of_size<1> : public mtp::TConst<ui8> {};

template<>
struct uint_of_size<2> : public mtp::TConst<ui16> {};

template<>
struct uint_of_size<3> : public mtp::TConst<ui32> {};

template<>
struct uint_of_size<4> : public mtp::TConst<ui64> {};

}

template<ui8 S>
using uint_of_size = typename details::uint_of_size<S>::type;

ECS_END_NS