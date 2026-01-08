#ifndef SIMUTPU_MXU_H
#define SIMUTPU_MXU_H

#include "simutpu/types.h"
#include "simutpu/memory.h"

namespace simutpu {

class MXU {
public:
    MXU();

    // Executes a GEMM operation: C = A * B
    // A is MxK, B is KxN, C is MxN
    // Reads/Writes directly from/to Memory
    void executeGEMM(Memory& memory, const Instruction& instr);

    // Returns the total cycles consumed by the last operation (or cumulative)
    uint64_t getCycles() const { return cycles_; }

private:
    uint64_t cycles_;

    // Simulating a fixed size systolic array internally
    // If the matrix is larger than this, a real TPU would tile it.
    // For this simple behavioral model, we will assume the hardware loops
    // to handle the full matrix size provided in the instruction.
    static constexpr int ARRAY_SIZE = 128;
};

} // namespace simutpu

#endif // SIMUTPU_MXU_H
