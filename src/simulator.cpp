#include "simutpu/simulator.h"
#include <iostream>

namespace simutpu {

Simulator::Simulator(size_t memory_size)
    : memory_(memory_size), pc_(0), halted_(false) {}

void Simulator::loadProgram(const std::vector<Instruction>& program) {
    instruction_memory_ = program;
    pc_ = 0;
    halted_ = false;
}

void Simulator::run() {
    std::cout << "[SimuTPU] Starting simulation..." << std::endl;

    while (!halted_ && pc_ < instruction_memory_.size()) {
        const Instruction& instr = instruction_memory_[pc_];

        switch (instr.opcode) {
            case Opcode::GEMM:
                mxu_.executeGEMM(memory_, instr);
                break;
            case Opcode::HALT:
                std::cout << "[SimuTPU] HALT encountered." << std::endl;
                halted_ = true;
                break;
            default:
                std::cerr << "[SimuTPU] Unknown opcode at PC=" << pc_ << std::endl;
                break;
        }

        if (!halted_) {
            pc_++;
        }
    }

    std::cout << "[SimuTPU] Simulation finished. Total MXU cycles: " << mxu_.getCycles() << std::endl;
}

} // namespace simutpu
