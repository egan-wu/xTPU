#pragma once
#include "simutpu/memory_types.h"
#include "simutpu/config.h"
#include <map>
#include <vector>
#include <mutex>
#include <optional>
#include <iostream>

namespace simutpu {

class MMU {
public:
    explicit MMU(const Config& config);

    // Translates VA to PA. Returns std::nullopt if page fault.
    std::optional<PhysicalAddress> translate(VirtualAddress vaddr, ProcessID pid, bool is_write);

    // Map a virtual page to a physical frame
    void mapPage(ProcessID pid, VirtualAddress vaddr, PhysicalAddress paddr, PageSize size, bool writable, bool shared);

    // Allocate a shared memory region (returns starting PADDR)
    PhysicalAddress allocateSharedMemory(size_t size);

    // Get statistics
    uint64_t getPageFaults() const { return page_faults_; }

private:
    Config config_;
    uint64_t page_faults_ = 0;

    // Simplistic Page Table storage: PID -> (VPN -> PTE)
    // In a real system this would be in memory, but for simulation we store it in a map.
    // We use a hierarchical map structure to simulate page tables if needed,
    // but a flat map is easier for functional simulation.
    // Key: (PID << 48) | (VirtualAddress >> 12) (for 4KB pages)
    // Note: This is an abstraction. Real HW walks memory.
    struct PageTable {
        std::map<uint64_t, PageTableEntry> entries;
    };

    std::map<ProcessID, PageTable> page_tables_;

    // Shared memory tracking
    PhysicalAddress next_shared_paddr_ = 0x100000000; // Start high
};

}
