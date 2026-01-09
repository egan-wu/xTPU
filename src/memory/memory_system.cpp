#include "simutpu/memory_system.h"
#include "simutpu/tracer.h"
#include <iostream>

namespace simutpu {

MemorySystem::MemorySystem(const Config& config)
    : config_(config), mmu_(config) {

    // Initialize DRAM
    dram_ = std::make_unique<SimpleMemory>("DRAM", config.dram_size, config.dram_latency);

    // Initialize Cache Models
    // Assuming 64-byte cache lines for now
    l1_cache_ = std::make_unique<CacheModel>(config.l1_size, 64, config.l1_latency);
    l2_cache_ = std::make_unique<CacheModel>(config.l2_size, 64, config.l2_latency);
}

void MemorySystem::loadPhysical(PhysicalAddress paddr, const std::vector<uint8_t>& data) {
    dram_->write(paddr, data);
}

MemoryResponse MemorySystem::access(const MemoryRequest& req, uint64_t current_cycle) {
    // Log Memory Access
    Tracer::getInstance().logMemory(current_cycle, req.vaddr, req.is_write);

    MemoryResponse resp;

    // 1. Virtual to Physical Translation
    auto paddr_opt = mmu_.translate(req.vaddr, req.pid, req.is_write);
    if (!paddr_opt.has_value()) {
        // Page Fault!
        // In a real system, OS handles this. Here we probably just fail or return error.
        throw std::runtime_error("Page Fault at address " + std::to_string(req.vaddr));
    }

    PhysicalAddress paddr = paddr_opt.value();

    // 2. Latency Simulation (L1 -> L2 -> DRAM)
    uint64_t total_latency = 0;

    // Check L1
    total_latency += l1_cache_->latency();
    if (l1_cache_->access(paddr)) {
        // L1 Hit
    } else {
        // L1 Miss -> Check L2
        total_latency += l2_cache_->latency();
        if (l2_cache_->access(paddr)) {
            // L2 Hit
        } else {
            // L2 Miss -> DRAM
            total_latency += dram_->latency();
        }
    }

    resp.latency = total_latency;

    // 3. Perform Actual Data Access (on DRAM)
    // In a pure functional model backed by DRAM:
    if (req.is_write) {
        dram_->write(paddr, req.data);
    } else {
        resp.data = dram_->read(paddr, req.size);
    }

    return resp;
}

}
