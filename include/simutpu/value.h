#pragma once
#include <variant>
#include <cstdint>
#include <stdexcept>
#include "simutpu/types.h"

namespace simutpu {

// Simple float16/bfloat16 storage (using uint16_t for storage)
// Real arithmetic would require a library or conversion to float.
using fp16 = uint16_t;
using bf16 = uint16_t;

struct Value {
    DataType type;
    std::variant<float, fp16, bf16, int8_t, int32_t> data;

    Value() : type(DataType::FP32), data(0.0f) {}

    // Constructors
    explicit Value(float v) : type(DataType::FP32), data(v) {}
    explicit Value(int32_t v) : type(DataType::INT4), data(v) {} // reusing int32 for small ints logic
    explicit Value(int8_t v) : type(DataType::INT8), data(v) {}

    // Conversion helper (simplified to float for behavior)
    float toFloat() const {
        if (std::holds_alternative<float>(data)) return std::get<float>(data);
        if (std::holds_alternative<int8_t>(data)) return static_cast<float>(std::get<int8_t>(data));
        if (std::holds_alternative<int32_t>(data)) return static_cast<float>(std::get<int32_t>(data));
        // FP16/BF16 - in simulation we might store them as bytes but compute as float
        return 0.0f;
    }

    static Value fromFloat(float v, DataType t) {
        Value val;
        val.type = t;
        // Mock quantization
        switch(t) {
            case DataType::FP32: val.data = v; break;
            case DataType::INT8: val.data = static_cast<int8_t>(v); break;
            default: val.data = v; break;
        }
        return val;
    }
};

}
