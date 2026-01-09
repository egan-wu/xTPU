#include "simutpu/config.h"
#include <fstream>
#include <iostream>

namespace simutpu {

Config Config::fromJson(const nlohmann::json& j) {
    Config c;
    if (j.contains("mxu_height")) c.mxu_height = j["mxu_height"];
    if (j.contains("mxu_width")) c.mxu_width = j["mxu_width"];

    // Memory Sizes
    if (j.contains("l1_size")) c.l1_size = j["l1_size"];
    if (j.contains("l2_size")) c.l2_size = j["l2_size"];
    if (j.contains("dram_size")) c.dram_size = j["dram_size"];

    // MMU Config
    if (j.contains("page_size_config")) {
        std::string psc = j["page_size_config"];
        if (psc == "4KB") c.page_size_config = PageSizeConfig::Pure4KB;
        else if (psc == "2MB") c.page_size_config = PageSizeConfig::Pure2MB;
        else c.page_size_config = PageSizeConfig::Mixed;
    }

    // Latency
    if (j.contains("l1_latency")) c.l1_latency = j["l1_latency"];
    if (j.contains("l2_latency")) c.l2_latency = j["l2_latency"];
    if (j.contains("dram_latency")) c.dram_latency = j["dram_latency"];
    if (j.contains("alu_latency")) c.alu_latency = j["alu_latency"];

    return c;
}

Config Config::load(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + filepath);
    }
    nlohmann::json j;
    file >> j;
    return fromJson(j);
}

}
