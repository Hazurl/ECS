#include <ecs/tests/Test.hpp>

#include <ecs/tests/Structs.hpp>
#include <ecs/container/RawArray.hpp>

ECS_BEGIN_NS
namespace test {

void Test::test_RawData() {
    section("RawData");
    
    using Def = RawData<vect3>;
    {
        // Type
        _equals(sizeof(Def), sizeof(vect3), "Size must be unchanged");
        _equals(alignof(Def), alignof(vect3), "Alignment must be unchanged");
    }
    {
        Def d{ 0, 1, 2 };
        vect3 v{ 0, 1, 2 };

        // Cast

        _equals(d.cast().a, v.a, "Braces initialization failed");
    }
    {
        Def d;
        d.cast().a = 42.42;
        d.cast().b = 1337.1337;
        d.cast().c = -1.1;
        vect3 v(42.42, 1337.1337, -1.1);

        // Cast

        _equals(d.cast().a, v.a, "Member affectation failed");
        _equals(d.cast().b, v.b, "Member affectation failed");
        _equals(d.cast().c, v.c, "Member affectation failed");
    }
    {
        Def ds[16];
        ds[7].cast().b = -42.42;

        vect3 vs[16];
        vs[7].b = -42.42;

        // Alignment & size & cast in array

        _equals(ds[7].cast().b, float(-42.42), "Member affectation in array failed");
        _equals(ds[7].cast().b, vs[7].b, "Array's RawData doesn't have the same behaviour of naked array");

        _equals(sizeof(ds), sizeof(vs), "Size of array must be unchanged");
        _equals(alignof(ds), alignof(vs), "Alignment of array must be unchanged");
    }
    {
        RawData<Elem1<float>> d ( 42 );
        RawData<Elem1<float>> e ( std::move(d) );

        // Move Cstr

        _equals(d.cast().a, e.cast().a, "Copy failed");
    }
    {
        RawData<Elem1<float>> d ( 42 );
        RawData<Elem1<float>> e ( 1337 );
        e = std::move(d);

        // Move affectation

        _equals(d.cast().a, e.cast().a, "Copy failed");
    }
    {
        RawData<Elem1<float>> d ( 1337 );
        RawData<Elem1<float>> e, f;

        // Copy raw & content
        
        e.copy_content(d);
        f.copy_raw(d);

        _equals(d.cast().a, e.cast().a, "Copy content failed");
        _equals(d.cast().a, f.cast().a, "Copy raw failed");
    }
}

}
ECS_END_NS