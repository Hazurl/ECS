#pragma once

#include <ecs/Config.hpp>
#include <mtp/Macro.h>

#include <string>

ECS_BEGIN_NS
namespace test {

#define v_str(x...) x, #x
#define _assert(cond, message) assert(v_str(cond), message)
#define _equals(a, b, message) equals(v_str(a), v_str(b), message)

class Test {
public:

    int run () {
        error = false;

        test_RawArray();

        return error ? 1 : 0;
    }

private:

    void test_RawArray();

    void section(std::string const& name) {
        std::cout << ">> " << MTP_CB_BLU << "Section " << name << MTP_C_RST << std::endl;
    }

    void assert(bool condition, std::string const& condition_str, std::string const& message) {
        if (!condition && (error = true)) {
            std::cout << MTP_CB_RED << "ASSERT FAILED " MTP_CB_YLW "'" << condition_str << "'" MTP_C_RST " :" << std::endl;
            std::cout << "\t"<< message << std::endl;
        }
    }

    template<typename A, typename B>
    void equals(A && a, std::string const& a_str, B && b, std::string const& b_str, std::string const& message) {
        if ((a != b) && (error = true)) {
            std::cout << MTP_CB_RED << "ASSERT EQUALS FAILED " MTP_CB_YLW "'" << a_str << " == " << b_str << "'" MTP_C_RST " -> " MTP_CB_YLW "'" << a << " == " << b << "'" MTP_C_RST " :" << std::endl;
            std::cout << "\t" << message << std::endl;
        }
    }

    bool error;

};

}
ECS_END_NS