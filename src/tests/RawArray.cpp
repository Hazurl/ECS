#include <ecs/tests/Test.hpp>

#include <ecs/tests/Structs.hpp>
#include <ecs/container/RawArray.hpp>

ECS_BEGIN_NS
namespace test {

void Test::test_RawArray() {
    section("RawArray");
    
    using Def = RawData<vect3>;

    assert(sizeof(Def) == sizeof(vect3), "RawData: size must be unchanged");
    assert(alignof(Def) == alignof(vect3), "RawData: alignment must be unchanged");
    
}

}
ECS_END_NS