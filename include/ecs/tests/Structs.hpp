#pragma once

#include <ecs/Config.hpp>

ECS_BEGIN_NS
namespace test {

struct Void {};

template<typename T>
struct Elem1 {
    T a;
};

template<typename T>
struct Elem2 {
    T a, b;
};

template<typename T>
struct Elem3 {
    T a, b, c;
};

template<typename T>
struct Elem4 {
    T a, b, c, d;
};

template<typename T, ui32 S>
struct WithArray {
    T a[S];
};

template<typename B>
struct BaseOf : public B {};

using vect3 = Elem3<float>;

}
ECS_END_NS