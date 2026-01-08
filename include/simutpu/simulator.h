#ifndef SIMUTPU_SIMULATOR_H
#define SIMUTPU_SIMULATOR_H

#include "simutpu/types.h"
#include "simutpu/memory.h"
#include "simutpu/mxu.h"
#include <vector>

namespace simutpu {

class Simulator {
public:
    Simulator(size_t memory_size);

    // Load instructions into the simulator
    void loadProgram(const std::vector<Instruction>& program);

    // Run the simulation until HALT or end of program
    void run();

    // Access memory for verification
    const Memory& getMemory() const { return memory_; }
    Memory& getMemory() { return memory_; }

private:
    Memory memory_;
    MXU mxu_;
    std::vector<Instruction> instruction_memory_;
    size_t pc_; // Program Counter
    bool halted_;
};

} // namespace simutpu

#endif // SIMUTPU_SIMULATOR_H
