#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <mutex>
#include "simutpu/isa.h"

namespace simutpu {

class Tracer {
public:
    static Tracer& getInstance() {
        static Tracer instance;
        return instance;
    }

    void open(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        file_.open(filename);
        if (file_.is_open()) {
            file_ << "[\n"; // Start JSON array
        }
    }

    void close() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (file_.is_open()) {
            file_ << "{}\n]"; // End JSON array (empty object to avoid trailing comma issue)
            file_.close();
        }
    }

    void logInstruction(uint64_t cycle, const Instruction& instr, size_t pc) {
        if (!file_.is_open()) return;
        std::lock_guard<std::mutex> lock(mutex_);
        file_ << "  {\"type\": \"instr\", \"cycle\": " << cycle
              << ", \"pc\": " << pc
              << ", \"opcode\": \"" << Instruction::opcodeToString(instr.opcode) << "\"},\n";
    }

    void logMemory(uint64_t cycle, uint64_t vaddr, bool is_write) {
        if (!file_.is_open()) return;
        std::lock_guard<std::mutex> lock(mutex_);
        file_ << "  {\"type\": \"mem\", \"cycle\": " << cycle
              << ", \"vaddr\": " << vaddr
              << ", \"op\": \"" << (is_write ? "W" : "R") << "\"},\n";
    }

private:
    Tracer() = default;
    ~Tracer() { close(); }
    std::ofstream file_;
    std::mutex mutex_;
};

}
