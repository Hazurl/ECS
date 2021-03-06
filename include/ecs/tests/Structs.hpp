#pragma once

#include <ecs/Config.hpp>

ECS_BEGIN_NS
namespace test {

struct Void {};

template<typename T>
struct Elem1 {
    Elem1() {}
    Elem1(T a) : a(a) {}
    T a;
};

template<typename T>
struct Elem2 {
    Elem2() {}
    Elem2(T a, T b) : a(a), b(b) {}
    T a, b;
};

template<typename T>
struct Elem3 {
    Elem3() {}
    Elem3(T a, T b, T c) : a(a), b(b), c(c) {}
    T a, b, c;
};

template<typename T>
struct Elem4 {
    Elem4() {}
    Elem4(T a, T b, T c, T d) : a(a), b(b), c(c), d(d) {}
    T a, b, c, d;
};

template<typename T, ui32 S>
struct WithArray {
    WithArray() {}
    template<typename A>
    WithArray(A const& a) : a(a) {}
    T a[S];
};

template<typename B>
struct BaseOf : public B {};

using vect3 = Elem3<float>;

}
ECS_END_NS