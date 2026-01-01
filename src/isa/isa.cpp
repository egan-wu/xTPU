#include "simutpu/isa.h"
#include <sstream>

namespace simutpu {

std::string Instruction::opcodeToString(Opcode op) {
    switch (op) {
        case Opcode::HALT: return "HALT";
        case Opcode::NOP: return "NOP";
        case Opcode::LI: return "LI";
        case Opcode::LOAD: return "LOAD";
        case Opcode::STORE: return "STORE";
        case Opcode::GEMM: return "GEMM";
        case Opcode::VEC_ADD: return "VEC_ADD";
        case Opcode::VEC_MUL: return "VEC_MUL";
        case Opcode::VEC_RELU: return "VEC_RELU";
        case Opcode::DUMP: return "DUMP";
        case Opcode::SYNC: return "SYNC";
        default: return "UNKNOWN";
    }
}

}
