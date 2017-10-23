#pragma once

#include <ecs/Config.hpp>
#include <mtp/Macro.h>

#include <string>

ECS_BEGIN_NS
namespace test {

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

    void assert(bool condition, std::string const& message) {
        if (!condition && (error = true))
            std::cout << MTP_CB_RED << "ASSERT: " << MTP_C_RST << message << std::endl;
    }

    bool error;

};

}
ECS_END_NS