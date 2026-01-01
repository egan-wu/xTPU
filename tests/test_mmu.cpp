#include <gtest/gtest.h>
#include "simutpu/mmu.h"

namespace simutpu {

class MMUTest : public ::testing::Test {
protected:
    Config config;
    void SetUp() override {
        config.page_size_config = PageSizeConfig::Mixed;
    }
};

TEST_F(MMUTest, BasicTranslation4KB) {
    MMU mmu(config);
    ProcessID pid = 1;
    VirtualAddress vaddr = 0x1000;
    PhysicalAddress paddr = 0x5000;

    mmu.mapPage(pid, vaddr, paddr, PageSize::Size4KB, true, false);

    auto result = mmu.translate(vaddr, pid, false);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), paddr);

    // Offset check
    auto result_offset = mmu.translate(vaddr + 100, pid, false);
    ASSERT_TRUE(result_offset.has_value());
    EXPECT_EQ(result_offset.value(), paddr + 100);
}

TEST_F(MMUTest, BasicTranslation2MB) {
    MMU mmu(config);
    ProcessID pid = 1;
    VirtualAddress vaddr = 0x200000; // 2MB aligned
    PhysicalAddress paddr = 0x800000;

    mmu.mapPage(pid, vaddr, paddr, PageSize::Size2MB, true, false);

    auto result = mmu.translate(vaddr + 0x1000, pid, false); // Offset 4KB inside 2MB page
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), paddr + 0x1000);
}

TEST_F(MMUTest, PageFault) {
    MMU mmu(config);
    ProcessID pid = 1;
    auto result = mmu.translate(0xdeadbeef, pid, false);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(mmu.getPageFaults(), 1);
}

TEST_F(MMUTest, PermissionFault) {
    MMU mmu(config);
    ProcessID pid = 1;
    mmu.mapPage(pid, 0x1000, 0x2000, PageSize::Size4KB, false, false); // Read-only

    auto read_res = mmu.translate(0x1000, pid, false);
    EXPECT_TRUE(read_res.has_value());

    auto write_res = mmu.translate(0x1000, pid, true);
    EXPECT_FALSE(write_res.has_value());
    EXPECT_EQ(mmu.getPageFaults(), 1);
}

TEST_F(MMUTest, SharedMemory) {
    MMU mmu(config);
    ProcessID pid1 = 1;
    ProcessID pid2 = 2;

    PhysicalAddress shared_pa = mmu.allocateSharedMemory(PAGE_SIZE_4KB);

    mmu.mapPage(pid1, 0x1000, shared_pa, PageSize::Size4KB, true, true);
    mmu.mapPage(pid2, 0x5000, shared_pa, PageSize::Size4KB, true, true);

    auto res1 = mmu.translate(0x1000, pid1, true);
    auto res2 = mmu.translate(0x5000, pid2, true);

    ASSERT_TRUE(res1.has_value());
    ASSERT_TRUE(res2.has_value());
    EXPECT_EQ(res1.value(), res2.value());
}

}
