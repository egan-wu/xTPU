#include <gtest/gtest.h>
#include "simutpu/mmu.h"

namespace simutpu {

class MMUBugTest : public ::testing::Test {
protected:
    Config config;
    void SetUp() override {
        config.page_size_config = PageSizeConfig::Mixed;
    }
};

TEST_F(MMUBugTest, HugePageBaseAddress) {
    MMU mmu(config);
    ProcessID pid = 1;
    // Map 2MB page at 0x200000 to PADDR 0x400000
    VirtualAddress vaddr = 0x200000;
    PhysicalAddress paddr = 0x400000;

    mmu.mapPage(pid, vaddr, paddr, PageSize::Size2MB, true, false);

    // Access exact base address
    auto res_base = mmu.translate(vaddr, pid, false);
    ASSERT_TRUE(res_base.has_value());
    EXPECT_EQ(res_base.value(), paddr);

    // Access offset
    auto res_offset = mmu.translate(vaddr + 4096, pid, false);
    ASSERT_TRUE(res_offset.has_value());
    EXPECT_EQ(res_offset.value(), paddr + 4096);
}

}
