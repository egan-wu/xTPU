cmake_minimum_required(VERSION 3.14)
project(SimuTPU VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Library
add_library(simutpu_lib
    src/memory.cpp
    src/mxu.cpp
    src/simulator.cpp
)

target_include_directories(simutpu_lib PUBLIC include)

# Example Executable
add_executable(simutpu_app examples/main.cpp)
target_link_libraries(simutpu_app PRIVATE simutpu_lib)
