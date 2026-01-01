#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <variant>
#include "simutpu/types.h"
#include "simutpu/memory_types.h"

namespace simutpu {

enum class Opcode {
    HALT,
    NOP,
    LI,     // Load Immediate
    LOAD,   // Load from Memory to Register/SRAM
    STORE,  // Store from Register/SRAM to Memory
    GEMM,   // Matrix Multiply
    VEC_ADD,
    VEC_MUL,
    VEC_RELU,
    DUMP,   // Dump memory to file: DUMP start_addr, length, "filename" (filename passed via register/ptr or hack)
    SYNC
};

struct Operand {
    enum class Type {
        Register,
        Immediate,
        Address
    };
    Type type;
    uint64_t value;
};

struct Instruction {
    Opcode opcode;
    std::vector<Operand> operands;

    // Helpers
    static std::string opcodeToString(Opcode op);
};

}
