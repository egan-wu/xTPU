#include <gtest/gtest.h>
#include "simutpu/memory_system.h"

namespace simutpu {

class MemorySystemTest : public ::testing::Test {
protected:
    Config config;
    void SetUp() override {
        config.l1_size = 4096; // Small L1
        config.l2_size = 16384; // Small L2
        config.dram_size = 1024 * 1024;
        config.l1_latency = 1;
        config.l2_latency = 10;
        config.dram_latency = 100;
        config.page_size_config = PageSizeConfig::Mixed;
    }
};

TEST_F(MemorySystemTest, BasicAccess) {
    MemorySystem mem(config);
    ProcessID pid = 1;
    VirtualAddress vaddr = 0x1000;
    PhysicalAddress paddr = 0x2000;

    // Map page
    mem.getMMU().mapPage(pid, vaddr, paddr, PageSize::Size4KB, true, false);

    // Write
    MemoryRequest write_req;
    write_req.vaddr = vaddr;
    write_req.size = 4;
    write_req.is_write = true;
    write_req.pid = pid;
    write_req.data = {0xAA, 0xBB, 0xCC, 0xDD};

    auto w_resp = mem.access(write_req);
    // First access is miss -> miss -> DRAM
    EXPECT_EQ(w_resp.latency, 1 + 10 + 100);

    // Read back
    MemoryRequest read_req;
    read_req.vaddr = vaddr;
    read_req.size = 4;
    read_req.is_write = false;
    read_req.pid = pid;

    auto r_resp = mem.access(read_req);
    EXPECT_EQ(r_resp.data, write_req.data);

    // Latency should be L1 hit now (since we accessed it on write)
    // Note: My simple cache model installs on access.
    EXPECT_EQ(r_resp.latency, 1);
}

TEST_F(MemorySystemTest, LatencySimulation) {
    MemorySystem mem(config);
    ProcessID pid = 1;
    VirtualAddress vaddr = 0x1000;
    PhysicalAddress paddr = 0x2000;
    mem.getMMU().mapPage(pid, vaddr, paddr, PageSize::Size4KB, true, false);

    // 1. Cold Access (Miss L1, Miss L2, Hit DRAM)
    MemoryRequest req;
    req.vaddr = vaddr;
    req.size = 1;
    req.is_write = false;
    req.pid = pid;

    auto resp1 = mem.access(req);
    EXPECT_EQ(resp1.latency, 1 + 10 + 100);

    // 2. Warm Access (Hit L1)
    auto resp2 = mem.access(req);
    EXPECT_EQ(resp2.latency, 1);

    // 3. Conflict Miss Simulation (Optional, depending on implementation details)
    // Since it's a simple mapped cache, accessing another address mapping to same index might evict.
}

}
