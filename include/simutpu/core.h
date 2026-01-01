#pragma once
#include "simutpu/config.h"
#include "simutpu/memory_system.h"
#include "simutpu/compute.h"
#include "simutpu/isa.h"
#include <vector>
#include <array>
#include <memory>

namespace simutpu {

class Core {
public:
    explicit Core(const Config& config);

    // Run a program
    void run(const std::vector<Instruction>& program);

    // Get statistics
    uint64_t getCycleCount() const { return cycle_count_; }

    // Accessors for verification
    MemorySystem& getMemorySystem() { return *memory_system_; }
    uint64_t getRegister(size_t index) const {
        if (index < registers_.size()) return registers_[index];
        return 0;
    }

    // Set up initial state (e.g. registers)
    void setRegister(size_t index, uint64_t value) {
        if (index < registers_.size()) registers_[index] = value;
    }

private:
    Config config_;
    std::unique_ptr<MemorySystem> memory_system_;
    std::unique_ptr<MXU> mxu_;
    std::unique_ptr<VPU> vpu_;

    // Scalar Registers R0-R31
    std::array<uint64_t, 32> registers_{};

    // Program Counter
    size_t pc_ = 0;

    // Statistics
    uint64_t cycle_count_ = 0;

    // Execution Helpers
    void executeInstruction(const Instruction& instr);
    uint64_t getOperandValue(const Operand& op);
};

}
