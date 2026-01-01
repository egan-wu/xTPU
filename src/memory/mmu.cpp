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
    // Note: If a 2MB page is mapped, its base address key is (vaddr & 2MB_MASK).
    // If vaddr happens to be aligned to 2MB, vpn4 == vpn2.
    // So if we find an entry at vpn4, it MIGHT be a 2MB page entry if the key collides.
    // However, in our mapPage logic, we store the key aligned to page size.
    // 2MB page at 0x200000 -> Key 0x200000.
    // 4KB page at 0x200000 -> Key 0x200000.
    // Since we use a single map, we can't map both at same address (which is correct).
    // But we need to check the entry type to calculate PADDR correctly.

    uint64_t vpn4 = vaddr & ~(PAGE_SIZE_4KB - 1);
    auto it4 = pt.entries.find(vpn4);
    if (it4 != pt.entries.end()) {
        PageTableEntry entry = it4->second;

        // If this entry claims to be huge, but we found it via 4KB alignment check,
        // it means we hit the base address of a huge page.
        // We should process it as a huge page.
        if (entry.huge_page) {
             // Fall through to 2MB logic or handle here.
             // Ideally we shouldn't have hit it here unless it was inserted with 4KB key?
             // No, mapPage inserts with vaddr aligned to size.
             // If vaddr=0x200000, key=0x200000. vpn4=0x200000. Match found.
             // So we MUST handle huge_page flag here.

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
