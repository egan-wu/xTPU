#include <gtest/gtest.h>
#include "simutpu/core.h"
#include "simutpu/isa_parser.h"

namespace simutpu {

class CoreTest : public ::testing::Test {
protected:
    Config config;
    void SetUp() override {
        config.page_size_config = PageSizeConfig::Mixed;
    }
};

TEST_F(CoreTest, BasicLoadStore) {
    Core core(config);

    // Setup Memory
    ProcessID pid = 1;
    VirtualAddress vaddr = 0x1000;
    PhysicalAddress paddr = 0x5000;
    core.getMemorySystem().getMMU().mapPage(pid, vaddr, paddr, PageSize::Size4KB, true, false);

    // Store 0xDEADBEEF to 0x1000
    // Load from 0x1000 to r1
    std::string script = R"(
        STORE 0xDEADBEEF, 0x1000
        LOAD r1, 0x1000
        HALT
    )";

    auto program = ISAParser::parse(script);

    core.run(program);

    EXPECT_EQ(core.getRegister(1), 0xDEADBEEF);
}

TEST_F(CoreTest, RegisterOps) {
    Core core(config);

    // Setup Memory
    ProcessID pid = 1;
    VirtualAddress vaddr = 0x1000;
    PhysicalAddress paddr = 0x5000;
    core.getMemorySystem().getMMU().mapPage(pid, vaddr, paddr, PageSize::Size4KB, true, false);

    // R2 = 0x1000
    core.setRegister(2, 0x1000);

    std::string script = R"(
        STORE 0xCAFEBABE, r2
        LOAD r3, r2
        HALT
    )";

    auto program = ISAParser::parse(script);
    core.run(program);

    EXPECT_EQ(core.getRegister(3), 0xCAFEBABE);
}

}
