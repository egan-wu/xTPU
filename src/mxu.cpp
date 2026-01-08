#include "simutpu/mxu.h"
#include <iostream>

namespace simutpu {

MXU::MXU() : cycles_(0) {}

void MXU::executeGEMM(Memory& memory, const Instruction& instr) {
    uint32_t M = instr.M;
    uint32_t K = instr.K;
    uint32_t N = instr.N;

    // Behavioral simulation of Matrix Multiplication
    // In a real hardware model, we would load tiles into local SRAM (Unified Buffer)
    // and then push them into the systolic array.
    // Here we read directly from global memory for simplicity.

    // Cycle estimation:
    // A systolic array takes K + M + N cycles roughly for a single tile pass.
    // For simplicity, we'll assume 1 MAC per cycle throughput per PE.
    // Total ops = M * N * K.
    // If we have ARRAY_SIZE * ARRAY_SIZE PEs, ideal cycles = (M * N * K) / (ARRAY_SIZE * ARRAY_SIZE)
    // We will just add a simple scalar for "latency".
    cycles_ += (static_cast<uint64_t>(M) * N * K) / (ARRAY_SIZE * ARRAY_SIZE) + ARRAY_SIZE;

    // Perform calculation
    // Assumes Row-Major layout
    for (uint32_t m = 0; m < M; ++m) {
        for (uint32_t n = 0; n < N; ++n) {
            DataType sum = 0;
            for (uint32_t k = 0; k < K; ++k) {
                Address addr_a = instr.addr_a + m * K + k;
                Address addr_b = instr.addr_b + k * N + n;

                DataType a_val = memory.read(addr_a);
                DataType b_val = memory.read(addr_b);

                sum += a_val * b_val;
            }
            Address addr_c = instr.addr_c + m * N + n;
            memory.write(addr_c, sum);
        }
    }

    std::cout << "[MXU] GEMM Complete: "
              << M << "x" << K << " * " << K << "x" << N
              << " -> stored at " << instr.addr_c << std::endl;
}

} // namespace simutpu
