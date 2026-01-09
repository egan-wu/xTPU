#pragma once
#include "simutpu/config.h"
#include "simutpu/memory_system.h"
#include "simutpu/memory_types.h"

namespace simutpu {

class DMAUnit {
public:
    explicit DMAUnit(const Config& config, MemorySystem& memory_system);

    struct Transfer {
        VirtualAddress src;
        VirtualAddress dst;
        size_t size;
        uint64_t start_cycle;
        uint64_t expected_latency;
    };

    // Starts a transfer. Returns estimated latency.
    // In a simple model, we might just return latency and block,
    // but for "parallelism" we need to track state.
    void startTransfer(VirtualAddress src, VirtualAddress dst, size_t size, uint64_t current_cycle);

    // Checks if DMA is busy at current_cycle
    bool isBusy(uint64_t current_cycle) const;

    // Returns the cycle when DMA will be free
    uint64_t getCompletionCycle() const { return completion_cycle_; }

private:
    Config config_;
    MemorySystem& memory_system_;
    uint64_t completion_cycle_ = 0;
};

}
