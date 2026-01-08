#include "simutpu/simulator.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>

// Helper to fill memory with some pattern
void fill_matrix(simutpu::Memory& mem, uint32_t start_addr, uint32_t rows, uint32_t cols, float start_val) {
    for (uint32_t i = 0; i < rows * cols; ++i) {
        mem.write(start_addr + i, start_val + static_cast<float>(i));
    }
}

// Helper to verify result
bool verify_matrix(const simutpu::Memory& mem, uint32_t addr, const std::vector<float>& expected, float tolerance = 1e-4) {
    for (size_t i = 0; i < expected.size(); ++i) {
        float val = mem.read(addr + i);
        if (std::abs(val - expected[i]) > tolerance) {
            std::cerr << "Mismatch at index " << i << ": expected " << expected[i] << ", got " << val << std::endl;
            return false;
        }
    }
    return true;
}

int main() {
    // 1. Initialize Simulator with 1MB memory
    simutpu::Simulator sim(1024 * 1024);

    // 2. Define Matrix Dimensions for a small test
    // A: 2x3, B: 3x2, C: 2x2
    uint32_t M = 2;
    uint32_t K = 3;
    uint32_t N = 2;

    simutpu::Address addr_a = 0;
    simutpu::Address addr_b = 100;
    simutpu::Address addr_c = 200;

    // 3. Load Input Data
    // Matrix A (2x3):
    // [1, 2, 3]
    // [4, 5, 6]
    std::vector<float> data_a = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    sim.getMemory().writeBlock(addr_a, data_a);

    // Matrix B (3x2):
    // [7, 8]
    // [9, 10]
    // [11, 12]
    std::vector<float> data_b = {7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f};
    sim.getMemory().writeBlock(addr_b, data_b);

    // 4. Create Program
    std::vector<simutpu::Instruction> program;

    // Instruction 1: GEMM
    simutpu::Instruction gemm_instr;
    gemm_instr.opcode = simutpu::Opcode::GEMM;
    gemm_instr.addr_a = addr_a;
    gemm_instr.addr_b = addr_b;
    gemm_instr.addr_c = addr_c;
    gemm_instr.M = M;
    gemm_instr.K = K;
    gemm_instr.N = N;
    program.push_back(gemm_instr);

    // Instruction 2: HALT
    program.push_back({simutpu::Opcode::HALT, 0, 0, 0, 0, 0, 0});

    sim.loadProgram(program);

    // 5. Run Simulation
    sim.run();

    // 6. Verify Output
    // Expected C = A * B
    // [1*7 + 2*9 + 3*11,  1*8 + 2*10 + 3*12]  = [7+18+33, 8+20+36] = [58, 64]
    // [4*7 + 5*9 + 6*11,  4*8 + 5*10 + 6*12]  = [28+45+66, 32+50+72] = [139, 154]
    std::vector<float> expected_c = {58.0f, 64.0f, 139.0f, 154.0f};

    if (verify_matrix(sim.getMemory(), addr_c, expected_c)) {
        std::cout << "SUCCESS: Verification Passed!" << std::endl;
        return 0;
    } else {
        std::cout << "FAILURE: Verification Failed!" << std::endl;
        return 1;
    }
}
