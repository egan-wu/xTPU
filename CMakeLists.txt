cmake_minimum_required(VERSION 3.14)
project(SimuTPU VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable testing
enable_testing()

# Fetch nlohmann_json
include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# Fetch GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Library setup
add_library(simutpu
    src/common/config.cpp
    src/memory/mmu.cpp
    src/memory/memory_system.cpp
    src/compute/compute.cpp
    src/isa/isa.cpp
    src/isa/isa_parser.cpp
    src/core/core.cpp
)

target_include_directories(simutpu PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(simutpu PUBLIC nlohmann_json::nlohmann_json)

# Compiler warnings
if(MSVC)
  target_compile_options(simutpu PRIVATE /W4)
else()
  target_compile_options(simutpu PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Tests
add_executable(simutpu_test
    tests/test_main.cpp
    tests/test_config.cpp
    tests/test_mmu.cpp
    tests/test_memory_system.cpp
    tests/test_compute.cpp
    tests/test_isa.cpp
    tests/test_core.cpp
)

target_link_libraries(simutpu_test PRIVATE simutpu GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(simutpu_test)

# Main Executable
add_executable(simutpu_cli src/main.cpp)
target_link_libraries(simutpu_cli PRIVATE simutpu)
