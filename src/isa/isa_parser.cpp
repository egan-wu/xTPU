#include "simutpu/isa_parser.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <map>

namespace simutpu {

namespace {
    std::string trim(const std::string& str) {
        // Remove comments starting with #
        std::string s = str;
        size_t comment_pos = s.find('#');
        if (comment_pos != std::string::npos) {
            s = s.substr(0, comment_pos);
        }

        size_t first = s.find_first_not_of(" \t\r\n");
        if (std::string::npos == first) return "";
        size_t last = s.find_last_not_of(" \t\r\n");
        return s.substr(first, (last - first + 1));
    }

    Opcode parseOpcode(const std::string& opStr) {
        if (opStr == "HALT") return Opcode::HALT;
        if (opStr == "NOP") return Opcode::NOP;
        if (opStr == "LI") return Opcode::LI;
        if (opStr == "LOAD") return Opcode::LOAD;
        if (opStr == "STORE") return Opcode::STORE;
        if (opStr == "GEMM") return Opcode::GEMM;
        if (opStr == "VEC_ADD") return Opcode::VEC_ADD;
        if (opStr == "VEC_MUL") return Opcode::VEC_MUL;
        if (opStr == "VEC_RELU") return Opcode::VEC_RELU;
        if (opStr == "DUMP") return Opcode::DUMP;
        if (opStr == "DMA_XFER") return Opcode::DMA_XFER;
        if (opStr == "SYNC") return Opcode::SYNC;
        throw std::runtime_error("Unknown opcode: " + opStr);
    }

    Operand parseOperand(const std::string& opStr) {
        Operand op;
        if (opStr.empty()) throw std::runtime_error("Empty operand");

        if (opStr[0] == 'r' || opStr[0] == 'R') {
            op.type = Operand::Type::Register;
            op.value = std::stoull(opStr.substr(1));
        } else if (opStr.rfind("0x", 0) == 0) {
            op.type = Operand::Type::Address; // Treat hex as address/immediate
            op.value = std::stoull(opStr, nullptr, 16);
        } else {
             // Assume immediate decimal
             op.type = Operand::Type::Immediate;
             op.value = std::stoull(opStr);
        }
        return op;
    }
}

std::vector<Instruction> ISAParser::parse(const std::string& script) {
    std::vector<Instruction> instructions;
    std::stringstream ss(script);
    std::string line;
    while (std::getline(ss, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue; // Skip empty or comment
        instructions.push_back(parseLine(trimmed));
    }
    return instructions;
}

Instruction ISAParser::parseLine(const std::string& line) {
    Instruction instr;
    std::stringstream ss(line);
    std::string segment;
    std::vector<std::string> parts;

    // Split by space, but handle commas if necessary (simplified parsing)
    // Actually, assembly often looks like: LOAD r1, 0x1000
    // Let's replace commas with spaces
    std::string cleanLine = line;
    std::replace(cleanLine.begin(), cleanLine.end(), ',', ' ');

    std::stringstream ssClean(cleanLine);
    while (ssClean >> segment) {
        parts.push_back(segment);
    }

    if (parts.empty()) throw std::runtime_error("Empty line");

    instr.opcode = parseOpcode(parts[0]);

    for (size_t i = 1; i < parts.size(); ++i) {
        instr.operands.push_back(parseOperand(parts[i]));
    }

    return instr;
}

}
