#pragma once
#include <cstdint>
#include <string>

namespace simutpu {

enum class DataType {
    FP32,
    FP16,
    BF16,
    INT8,
    INT4
};

inline size_t getByteSize(DataType type) {
    switch (type) {
        case DataType::FP32: return 4;
        case DataType::FP16: return 2;
        case DataType::BF16: return 2;
        case DataType::INT8: return 1;
        case DataType::INT4: return 1; // Packed, but 1 byte minimal addressable
        default: return 0;
    }
}

inline std::string toString(DataType type) {
    switch (type) {
        case DataType::FP32: return "FP32";
        case DataType::FP16: return "FP16";
        case DataType::BF16: return "BF16";
        case DataType::INT8: return "INT8";
        case DataType::INT4: return "INT4";
        default: return "Unknown";
    }
}

}
