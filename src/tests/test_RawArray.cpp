#include <ecs/tests/Test.hpp>

#include <ecs/tests/Structs.hpp>
#include <ecs/container/RawArray.hpp>

ECS_BEGIN_NS
namespace test {

void Test::test_RawArray() {        
    section("RawArray");
    
    using SRA = SizedRawArray<vect3, 42>;
    using SA = vect3[42];
    using RA = RawArray<vect3>;
    using A = vect3*;

    {
        // Structure 

        _equals(sizeof(SRA), sizeof(SA), "Size must be unchanged");
        _equals(alignof(SRA), alignof(SA), "Alignment must be unchanged");
        _equals(sizeof(RA), sizeof(A), "Size must be unchanged");
        _equals(alignof(RA), alignof(A), "Alignment must be unchanged");
    }
    {
        SRA sra;
        sra[7] = vect3(42.42, 1337, -1.1);
        RA ra(42);
        ra[7] = vect3(42.42, 1337, -1.1);
        vect3 v(42.42, 1337, -1.1);

        // Element in array, operator []

        _equals(sizeof(sra[7]), sizeof(v), "Size in array must be unchanged");
        _equals(alignof(sra[7]), alignof(v), "Alignment in array must be unchanged");
        _equals(sizeof(ra[7]), sizeof(v), "Size in array must be unchanged");
        _equals(alignof(ra[7]), alignof(v), "Alignment in array must be unchanged");

        _equals(sra[7].cast().a, v.a, "Member affectation failed");
        _equals(sra[7].cast().b, v.b, "Member affectation failed");
        _equals(sra[7].cast().c, v.c, "Member affectation failed");
        _equals(ra[7].cast().a, v.a, "Member affectation failed");
        _equals(ra[7].cast().b, v.b, "Member affectation failed");
        _equals(ra[7].cast().c, v.c, "Member affectation failed");
    }
    {
        RA ra(42);
        SRA sra;
        for (int i = 0; i < 42; ++i) {
            if (i % 3 == 0) {
                ra[i] = vect3(i * 42.42, -i, i + 1337.1337);
                sra[i] = vect3(i * 42.42, -i, i + 1337.1337);
            } else {
                ra[i] = vect3();
                sra[i] = vect3();
            }
        }

        // operaor [] and Data_t* conversion

        auto dra = static_cast<RA::Data_t*>(ra);
        auto dsra = static_cast<SRA::Data_t*>(sra);
        for (int i = 0; i < 42; ++i) {
            _equals(dra[i].cast().a, dsra[i].cast().a, "Member affectation failed");
            _equals(dra[i].cast().b, dsra[i].cast().b, "Member affectation failed");
            _equals(dra[i].cast().c, dsra[i].cast().c, "Member affectation failed");
        }
    }
    {
        RA ra(42);
        RA old_ra(42);
        SRA sra;
        SRA old_sra;
        for (int i = 0; i < 42; ++i) {
            if (i % 3 == 0) {
                ra[i] = vect3(i * 42.42, -i, i + 1337.1337);
                old_ra[i] = vect3(i * 42.42, -i, i + 1337.1337);
                sra[i] = vect3(i * 42.42, -i, i + 1337.1337);
                old_sra[i] = vect3(i * 42.42, -i, i + 1337.1337);
            } else {
                ra[i] = vect3();
                old_ra[i] = vect3();
                sra[i] = vect3();
                old_sra[i] = vect3();
            }
        }

        RA _ra = std::move(old_ra);
        SRA _sra = std::move(old_sra);

        // Copy
        
        for (int i = 0; i < 42; ++i) {
            _equals(ra[i].cast().a, _ra[i].cast().a, "Move Copy failed");
            _equals(ra[i].cast().b, _ra[i].cast().b, "Move Copy failed");
            _equals(ra[i].cast().c, _ra[i].cast().c, "Move Copy failed");

            _equals(sra[i].cast().a, _sra[i].cast().a, "Move Copy failed");
            _equals(sra[i].cast().b, _sra[i].cast().b, "Move Copy failed");
            _equals(sra[i].cast().c, _sra[i].cast().c, "Move Copy failed");
        }
    }
    {
        RA ra(42);
        SRA sra;
        for (int i = 0; i < 42; ++i) {
            if (i % 3 == 0) {
                ra[i] = vect3(i * 42.42, -i, i + 1337.1337);
                sra[i] = vect3(i * 42.42, -i, i + 1337.1337);
            } else {
                ra[i] = vect3();
                sra[i] = vect3();
            }
        }

        RA c_ra(42);
        c_ra.copy_content(ra, 42);
        RA r_ra(42);
        r_ra.copy_raw(ra, 42);
        SRA c_sra;
        c_sra.copy_content(sra);
        SRA r_sra;
        r_sra.copy_raw(sra);

        // Copy Content / Raw
        
        for (int i = 0; i < 42; ++i) {
            _equals(ra[i].cast().a, c_ra[i].cast().a, "Copy Content failed");
            _equals(ra[i].cast().b, c_ra[i].cast().b, "Copy Content failed");
            _equals(ra[i].cast().c, c_ra[i].cast().c, "Copy Content failed");

            _equals(ra[i].cast().a, r_ra[i].cast().a, "Copy Raw failed");
            _equals(ra[i].cast().b, r_ra[i].cast().b, "Copy Raw failed");
            _equals(ra[i].cast().c, r_ra[i].cast().c, "Copy Raw failed");

            _equals(sra[i].cast().a, c_sra[i].cast().a, "Copy Content failed");
            _equals(sra[i].cast().b, c_sra[i].cast().b, "Copy Content failed");
            _equals(sra[i].cast().c, c_sra[i].cast().c, "Copy Content failed");

            _equals(sra[i].cast().a, r_sra[i].cast().a, "Copy Raw failed");
            _equals(sra[i].cast().b, r_sra[i].cast().b, "Copy Raw failed");
            _equals(sra[i].cast().c, r_sra[i].cast().c, "Copy Raw failed");
        }
    }
}

}
ECS_END_NS