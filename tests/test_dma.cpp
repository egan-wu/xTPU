#include <gtest/gtest.h>
#include "simutpu/dma.h"
#include "simutpu/core.h"
#include "simutpu/isa_parser.h"

namespace simutpu {

class DMATest : public ::testing::Test {
protected:
    Config config;
    void SetUp() override {
        config.page_size_config = PageSizeConfig::Mixed;
        config.dram_latency = 100;
    }
};

TEST_F(DMATest, TransferLogic) {
    MemorySystem mem(config);
    DMAUnit dma(config, mem);

    // Map pages
    ProcessID pid = 1;
    mem.getMMU().mapPage(pid, 0x1000, 0x1000, PageSize::Size4KB, true, false);
    mem.getMMU().mapPage(pid, 0x2000, 0x2000, PageSize::Size4KB, true, false);

    // Write data to src
    MemoryRequest write_req;
    write_req.vaddr = 0x1000;
    write_req.size = 4;
    write_req.is_write = true;
    write_req.pid = pid;
    write_req.data = {0xA, 0xB, 0xC, 0xD};
    mem.access(write_req);

    // Start Transfer
    // Latency = 100 + 4 = 104 cycles (simplified model)
    uint64_t start = 10;
    dma.startTransfer(0x1000, 0x2000, 4, start);

    EXPECT_TRUE(dma.isBusy(start));
    EXPECT_TRUE(dma.isBusy(start + 50));
    EXPECT_FALSE(dma.isBusy(start + 104));
    EXPECT_EQ(dma.getCompletionCycle(), start + 104);

    // Check Data at DST
    MemoryRequest read_req;
    read_req.vaddr = 0x2000;
    read_req.size = 4;
    read_req.is_write = false;
    read_req.pid = pid;
    auto resp = mem.access(read_req);

    EXPECT_EQ(resp.data, write_req.data);
}

TEST_F(DMATest, IntegrationWithCore) {
    Core core(config);
    ProcessID pid = 1;
    // Map
    core.getMemorySystem().getMMU().mapPage(pid, 0x1000, 0x1000, PageSize::Size4KB, true, false);
    core.getMemorySystem().getMMU().mapPage(pid, 0x2000, 0x2000, PageSize::Size4KB, true, false);

    // Script:
    // Write to 0x1000
    // Start DMA 0x1000 -> 0x2000
    // SYNC
    // Load from 0x2000

    std::string script = R"(
        LI r1, 0x1000
        LI r2, 0x2000
        LI r3, 0xDEADBEEF
        STORE r3, r1

        # DMA XFER src=r1, dst=r2, size=8
        DMA_XFER r1, r2, 8

        # At this point, DMA is running. Core continues instantly.
        # We can add a NOP
        NOP

        # Wait for completion
        SYNC

        # Verify
        LOAD r4, r2
        HALT
    )";

    auto prog = ISAParser::parse(script);
    core.run(prog);

    EXPECT_EQ(core.getRegister(4), 0xDEADBEEF);

    // Check Cycle Count
    // Dispatch LI/STORE/DMA: ~5-10 cycles
    // DMA Latency: 100 + 8 = 108
    // SYNC should jump cycle count to approx 108 + dispatch start time
    EXPECT_GT(core.getCycleCount(), 100);
}

}
