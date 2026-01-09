#include "simutpu/dma.h"
#include <algorithm>

namespace simutpu {

DMAUnit::DMAUnit(const Config& config, MemorySystem& memory_system)
    : config_(config), memory_system_(memory_system) {}

void DMAUnit::startTransfer(VirtualAddress src, VirtualAddress dst, size_t size, uint64_t current_cycle) {
    // 1. Calculate Latency
    // Simple model: startup_latency + size / bandwidth_factor
    // Let's assume DRAM bandwidth bottleneck.
    // E.g., 1 byte per cycle for simplicity unless config specifies otherwise.
    // config.dram_latency is access latency.

    // We perform the copy "instantaneously" in behavioral simulation but model the time.
    // Or we could actually perform it byte-by-byte in a step() function.
    // For behavioral, instant copy + time accounting is standard.

    // Perform Copy
    std::vector<uint8_t> buffer(size);
    // Read
    for (size_t i = 0; i < size; ++i) {
        MemoryRequest req;
        req.vaddr = src + i;
        req.size = 1;
        req.is_write = false;
        req.pid = 1;
        auto resp = memory_system_.access(req, current_cycle);
        buffer[i] = resp.data[0];
    }

    // Write
    for (size_t i = 0; i < size; ++i) {
        MemoryRequest req;
        req.vaddr = dst + i;
        req.size = 1;
        req.is_write = true;
        req.pid = 1;
        req.data = {buffer[i]};
        memory_system_.access(req, current_cycle);
    }

    // Calculate Latency
    // Assumes linear bandwidth
    uint64_t latency = config_.dram_latency + size; // Simplified

    if (current_cycle < completion_cycle_) {
        // Queueing effect (serialized DMA)
        completion_cycle_ += latency;
    } else {
        completion_cycle_ = current_cycle + latency;
    }
}

bool DMAUnit::isBusy(uint64_t current_cycle) const {
    return current_cycle < completion_cycle_;
}

}
