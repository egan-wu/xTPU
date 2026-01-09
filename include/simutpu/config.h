#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace simutpu {

enum class PageSizeConfig {
    Pure4KB,
    Pure2MB,
    Mixed
};

struct Config {
    // Array Size
    int mxu_height = 16;
    int mxu_width = 16;

    // Memory Hierarchy Sizes (in bytes)
    size_t l1_size = 1024 * 1024;        // 1MB (SRAM/L1)
    size_t l2_size = 16 * 1024 * 1024;   // 16MB (L2)
    size_t dram_size = 1024 * 1024 * 1024; // 1GB (DRAM)

    // MMU Configuration
    PageSizeConfig page_size_config = PageSizeConfig::Mixed;

    // Latency (in cycles)
    int l1_latency = 1;
    int l2_latency = 10;
    int dram_latency = 100;
    int alu_latency = 1;

    static Config fromJson(const nlohmann::json& j);
    static Config load(const std::string& filepath);
};

}
