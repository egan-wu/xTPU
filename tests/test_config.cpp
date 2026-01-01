#include <gtest/gtest.h>
#include "simutpu/config.h"
#include <fstream>
#include <cstdio>

namespace simutpu {

TEST(ConfigTest, LoadDefaults) {
    nlohmann::json j = nlohmann::json::object();
    Config c = Config::fromJson(j);
    EXPECT_EQ(c.mxu_height, 16);
    EXPECT_EQ(c.l1_size, 1024 * 1024);
    EXPECT_EQ(c.page_size_config, PageSizeConfig::Mixed);
}

TEST(ConfigTest, LoadCustom) {
    nlohmann::json j;
    j["mxu_height"] = 32;
    j["l1_size"] = 2048;
    j["page_size_config"] = "4KB";

    Config c = Config::fromJson(j);
    EXPECT_EQ(c.mxu_height, 32);
    EXPECT_EQ(c.l1_size, 2048);
    EXPECT_EQ(c.page_size_config, PageSizeConfig::Pure4KB);
}

TEST(ConfigTest, LoadFile) {
    std::string filename = "test_config.json";
    {
        std::ofstream out(filename);
        out << R"({"mxu_height": 64, "page_size_config": "2MB"})";
    }

    Config c = Config::load(filename);
    EXPECT_EQ(c.mxu_height, 64);
    EXPECT_EQ(c.page_size_config, PageSizeConfig::Pure2MB);

    std::remove(filename.c_str());
}

}
