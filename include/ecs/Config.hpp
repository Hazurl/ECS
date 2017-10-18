#pragma once

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

ECS_END_NS