#pragma once
#include <cstdint>
#include <vector>
#include <variant>

namespace simutpu {

using VirtualAddress = uint64_t;
using PhysicalAddress = uint64_t;
using ProcessID = uint32_t;
using UserID = uint32_t;

constexpr uint64_t PAGE_SIZE_4KB = 4096;
constexpr uint64_t PAGE_SIZE_2MB = 2 * 1024 * 1024;

enum class PageSize {
    Size4KB,
    Size2MB
};

// Simple Page Table Entry
struct PageTableEntry {
    PhysicalAddress pfn : 40; // Physical Frame Number
    uint64_t present : 1;
    uint64_t writable : 1;
    uint64_t user : 1; // User/Supervisor
    uint64_t huge_page : 1; // 1 if 2MB page
    uint64_t shared : 1; // Shared memory
    uint64_t ignored : 19;

    PageTableEntry() : pfn(0), present(0), writable(0), user(0), huge_page(0), shared(0), ignored(0) {}
};

// Memory Request
struct MemoryRequest {
    VirtualAddress vaddr;
    size_t size;
    bool is_write;
    ProcessID pid;

    // For writes
    std::vector<uint8_t> data;
};

// Memory Response
struct MemoryResponse {
    std::vector<uint8_t> data; // For reads
    uint64_t latency;
};

}
