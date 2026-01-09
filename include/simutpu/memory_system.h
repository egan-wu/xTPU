#pragma once
#include "simutpu/mmu.h"
#include "simutpu/memory_device.h"
#include <memory>
#include <map>

namespace simutpu {

class MemorySystem {
public:
    explicit MemorySystem(const Config& config);

    // Main interface for the Core
    MemoryResponse access(const MemoryRequest& req, uint64_t current_cycle = 0);

    // Helpers to access components directly for testing/setup
    MMU& getMMU() { return mmu_; }

    // Debug helpers
    void loadPhysical(PhysicalAddress paddr, const std::vector<uint8_t>& data);

private:
    Config config_;
    MMU mmu_;

    // Hierarchy
    std::unique_ptr<SimpleMemory> dram_;
    std::unique_ptr<SimpleMemory> l2_;
    std::unique_ptr<SimpleMemory> l1_;

    // For simulation purpose, we need to map physical addresses to specific memory devices.
    // In a real system, L1/L2 are caches. Here, we treat them as addressable memories or caches?
    // The prompt says "Memory structure should have three parts, DRAM, L2 memory stands between L1 memory and DRAM, and L1 memory".
    // It also implies traffic/latency simulation.
    //
    // If they are caches, they are transparent to software (software sees one Physical Address Space).
    // If they are distinct address spaces (like SRAM in TPU), they are explicit.
    // The prompt mentions "Host Memory (DRAM)", "Device Memory (HBM)", "On-Chip Buffer (SRAM)".
    // But the *new* requirement calls for DRAM -> L2 -> L1 hierarchy.
    //
    // Let's model this as a Cache Hierarchy for the main memory path.
    // However, implementing a full cache coherency/eviction policy might be overkill unless requested.
    // "Models data-movement latency and compute throughput" - README.
    //
    // Proposal:
    // Physical Memory (DRAM) is the "backing store".
    // L1 and L2 are modeled as caches that check for hits/misses to calculate latency.
    //
    // But wait, the original README said "On-Chip Buffer (SRAM)".
    // Let's assume the user wants a standard hierarchy where access goes L1 -> L2 -> DRAM.
    // We will simulate latency based on where the data "is".
    // Since we don't want to implement a complex replacement policy right now,
    // we can implement a "Inclusive Cache Simulation" model where we just add latencies.
    // Or, simpler: All data lives in DRAM (functional), but we calculate latency based on hits (probabilistic or simple tag check).

    // Actually, let's implement a simplified cache model.
    // We need tags.

    struct CacheLine {
        bool valid = false;
        uint64_t tag = 0;
        bool dirty = false;
    };

    // Simple direct-mapped or set-associative cache metadata
    // We won't store data in L1/L2 for this behavioral model, just valid/tags to simulate hits.
    // The actual data will always be read/written to DRAM for correctness simplicity.
    // (Unless the user explicitly asks for detailed cache data modeling).

    class CacheModel {
    public:
        CacheModel(size_t size, size_t line_size, int latency)
            : size_(size), line_size_(line_size), latency_(latency) {
            num_lines_ = size / line_size;
            tags_.resize(num_lines_);
        }

        // Returns true if hit
        bool access(PhysicalAddress paddr) {
            uint64_t index = (paddr / line_size_) % num_lines_;
            uint64_t tag = paddr / (line_size_ * num_lines_);

            if (tags_[index].valid && tags_[index].tag == tag) {
                return true;
            }

            // Miss - install line
            tags_[index].valid = true;
            tags_[index].tag = tag;
            return false;
        }

        int latency() const { return latency_; }

    private:
        size_t size_;
        size_t line_size_;
        int latency_;
        size_t num_lines_;
        std::vector<CacheLine> tags_;
    };

    std::unique_ptr<CacheModel> l1_cache_;
    std::unique_ptr<CacheModel> l2_cache_;

    // We treat DRAM as the data holder.
    // If the system is strictly scratchpad based (TPU style), L1 = SRAM.
    // The prompt says "Memory structure should have three parts... L2 memory stands between L1 memory and DRAM".
    // This sounds like a cache hierarchy.

};

}
