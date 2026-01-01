#include "simutpu/mmu.h"
#include <stdexcept>

namespace simutpu {

MMU::MMU(const Config& config) : config_(config) {}

std::optional<PhysicalAddress> MMU::translate(VirtualAddress vaddr, ProcessID pid, bool is_write) {
    auto& pt = page_tables_[pid];

    // Try 4KB page lookup first
    // In a mixed setup, we need to check if it's mapped as 2MB or 4KB.
    // Our map key storage needs to be clever.
    // Let's assume we store the base address of the page as the key.

    // Simplification for simulation:
    // We check exact matches in our map.

    // 1. Check for 4KB page
    uint64_t vpn4 = vaddr & ~(PAGE_SIZE_4KB - 1);
    auto it4 = pt.entries.find(vpn4);
    if (it4 != pt.entries.end()) {
        PageTableEntry entry = it4->second;
        if (!entry.present) {
            page_faults_++;
            return std::nullopt;
        }
        if (is_write && !entry.writable) {
            // Permission fault
             page_faults_++; // Treat as fault for now
            return std::nullopt;
        }
        return (entry.pfn * PAGE_SIZE_4KB) + (vaddr % PAGE_SIZE_4KB);
    }

    // 2. Check for 2MB page
    // Ensure we are allowed to use 2MB pages
    if (config_.page_size_config != PageSizeConfig::Pure4KB) {
        uint64_t vpn2 = vaddr & ~(PAGE_SIZE_2MB - 1);
        auto it2 = pt.entries.find(vpn2);
        if (it2 != pt.entries.end()) {
             PageTableEntry entry = it2->second;
             if (entry.huge_page) {
                if (!entry.present) {
                    page_faults_++;
                    return std::nullopt;
                }
                if (is_write && !entry.writable) {
                    page_faults_++;
                    return std::nullopt;
                }
                return (entry.pfn * PAGE_SIZE_2MB) + (vaddr % PAGE_SIZE_2MB);
             }
        }
    }

    page_faults_++;
    return std::nullopt;
}

void MMU::mapPage(ProcessID pid, VirtualAddress vaddr, PhysicalAddress paddr, PageSize size, bool writable, bool shared) {
    if (config_.page_size_config == PageSizeConfig::Pure4KB && size == PageSize::Size2MB) {
        throw std::runtime_error("Cannot map 2MB page in Pure4KB mode");
    }
    if (config_.page_size_config == PageSizeConfig::Pure2MB && size == PageSize::Size4KB) {
        throw std::runtime_error("Cannot map 4KB page in Pure2MB mode");
    }

    PageTableEntry entry;
    entry.present = 1;
    entry.writable = writable ? 1 : 0;
    entry.shared = shared ? 1 : 0;

    uint64_t key = 0;

    if (size == PageSize::Size2MB) {
        entry.huge_page = 1;
        entry.pfn = paddr / PAGE_SIZE_2MB;
        key = vaddr & ~(PAGE_SIZE_2MB - 1);
    } else {
        entry.huge_page = 0;
        entry.pfn = paddr / PAGE_SIZE_4KB;
        key = vaddr & ~(PAGE_SIZE_4KB - 1);
    }

    page_tables_[pid].entries[key] = entry;
}

PhysicalAddress MMU::allocateSharedMemory(size_t size) {
    // Simple bump allocator for simulated shared physical memory
    PhysicalAddress start = next_shared_paddr_;
    next_shared_paddr_ += size;
    // Align to 2MB for safety
    if (next_shared_paddr_ % PAGE_SIZE_2MB != 0) {
        next_shared_paddr_ = (next_shared_paddr_ + PAGE_SIZE_2MB - 1) & ~(PAGE_SIZE_2MB - 1);
    }
    return start;
}

}
