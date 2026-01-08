#ifndef SIMUTPU_TYPES_H
#define SIMUTPU_TYPES_H

#include <cstdint>
#include <vector>
#include <iostream>

namespace simutpu {

// Using float for simplicity as per requirements, but can be abstracted later
using DataType = float;
using Address = uint32_t;

enum class Opcode {
    HALT,
    GEMM,   // General Matrix Multiply
    LOAD,   // Just a placeholder for now
    STORE   // Just a placeholder for now
};

struct Instruction {
    Opcode opcode;

    // Operands for GEMM: C = A * B
    // Storing addresses and dimensions
    Address addr_a;
    Address addr_b;
    Address addr_c;

    uint32_t M; // Rows of A / Rows of C
    uint32_t K; // Cols of A / Rows of B
    uint32_t N; // Cols of B / Cols of C
};

} // namespace simutpu

#endif // SIMUTPU_TYPES_H
