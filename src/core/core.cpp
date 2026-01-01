#include "simutpu/core.h"
#include "simutpu/tracer.h"
#include <iostream>
#include <stdexcept>
#include <fstream>

namespace simutpu {

Core::Core(const Config& config) : config_(config) {
    memory_system_ = std::make_unique<MemorySystem>(config);
    mxu_ = std::make_unique<MXU>(config);
    vpu_ = std::make_unique<VPU>(config);
    dma_ = std::make_unique<DMAUnit>(config, *memory_system_);
}

void Core::run(const std::vector<Instruction>& program) {
    pc_ = 0;
    while (pc_ < program.size()) {
        const auto& instr = program[pc_];
        if (instr.opcode == Opcode::HALT) {
            break;
        }
        executeInstruction(instr);
        pc_++;
    }
}

uint64_t Core::getOperandValue(const Operand& op) {
    if (op.type == Operand::Type::Register) {
        if (op.value >= registers_.size()) {
             throw std::runtime_error("Invalid register index");
        }
        return registers_[op.value];
    } else {
        return op.value;
    }
}

void Core::executeInstruction(const Instruction& instr) {
    // Log Trace
    Tracer::getInstance().logInstruction(cycle_count_, instr, pc_);

    // Basic Fetch latency
    cycle_count_ += 1;

    switch (instr.opcode) {
        case Opcode::NOP:
            break;
        case Opcode::LI: {
            // LI dst_reg, immediate
            if (instr.operands.size() < 2) throw std::runtime_error("LI requires 2 operands");
            size_t reg_idx = instr.operands[0].value;
            uint64_t val = instr.operands[1].value; // Assumes parser tagged immediate or address
            setRegister(reg_idx, val);
            cycle_count_ += 1;
            break;
        }
        case Opcode::DMA_XFER: {
            // DMA_XFER src, dst, size
            if (instr.operands.size() < 3) throw std::runtime_error("DMA_XFER requires 3 operands");
            VirtualAddress src = getOperandValue(instr.operands[0]);
            VirtualAddress dst = getOperandValue(instr.operands[1]);
            uint64_t size = getOperandValue(instr.operands[2]);

            dma_->startTransfer(src, dst, size, cycle_count_);
            // Core dispatch latency (DMA is async)
            cycle_count_ += 1;
            break;
        }
        case Opcode::LOAD: {
            // LOAD dst_reg, address
            if (instr.operands.size() < 2) throw std::runtime_error("LOAD requires 2 operands");
            size_t reg_idx = instr.operands[0].value; // assuming always register
            VirtualAddress vaddr = getOperandValue(instr.operands[1]);

            MemoryRequest req;
            req.vaddr = vaddr;
            req.size = 8; // Load 64-bit word
            req.is_write = false;
            req.pid = 1; // Default PID for single-core sim

            auto resp = memory_system_->access(req, cycle_count_);

            // Write back to register (simplified endianness)
            uint64_t value = 0;
            for(int i=0; i<8; ++i) {
                value |= (uint64_t)resp.data[i] << (i*8);
            }

            setRegister(reg_idx, value);
            cycle_count_ += resp.latency;
            break;
        }
        case Opcode::STORE: {
            // STORE src_reg, address
            if (instr.operands.size() < 2) throw std::runtime_error("STORE requires 2 operands");
            uint64_t value = getOperandValue(instr.operands[0]);
            VirtualAddress vaddr = getOperandValue(instr.operands[1]);

            MemoryRequest req;
            req.vaddr = vaddr;
            req.size = 8;
            req.is_write = true;
            req.pid = 1;

            req.data.resize(8);
            for(int i=0; i<8; ++i) {
                req.data[i] = (value >> (i*8)) & 0xFF;
            }

            auto resp = memory_system_->access(req, cycle_count_);
            cycle_count_ += resp.latency;
            break;
        }
        case Opcode::GEMM: {
            // Placeholder: In a real sim, operands point to memory buffers or matrix registers
            // Here we just advance time
            cycle_count_ += mxu_->latency();
            break;
        }
        case Opcode::VEC_ADD: {
            if (instr.operands.size() < 3) throw std::runtime_error("VEC_ADD requires 3 operands");
             // VEC_ADD dst, src1, src2
            uint64_t val1 = getOperandValue(instr.operands[1]);
            uint64_t val2 = getOperandValue(instr.operands[2]);
            size_t dst = instr.operands[0].value;

            // Interpret as float for simulation demo
            // In a real typed system, we would check status flags or instruction suffixes (VEC_ADD.F32)
            // Here we assume simple casting for demo purpose.
            // If values look like integers, add them.
            setRegister(dst, val1 + val2);

            cycle_count_ += vpu_->latency();
            break;
        }
        case Opcode::VEC_MUL: {
            if (instr.operands.size() < 3) throw std::runtime_error("VEC_MUL requires 3 operands");
            uint64_t val1 = getOperandValue(instr.operands[1]);
            uint64_t val2 = getOperandValue(instr.operands[2]);
            size_t dst = instr.operands[0].value;
            setRegister(dst, val1 * val2);
            cycle_count_ += vpu_->latency();
            break;
        }
        case Opcode::VEC_RELU: {
             // For scalar registers in this simple integration, acts like scalar
            cycle_count_ += vpu_->latency();
            break;
        }
        case Opcode::DUMP: {
            // DUMP start_addr, length (filename hardcoded to dump.csv for simplicity or handled via convention)
            // Implementation: DUMP 0x1000, 16 -> dumps 16 bytes at 0x1000 to "memory_dump.csv"
            if (instr.operands.size() < 2) throw std::runtime_error("DUMP requires 2 operands");
            VirtualAddress vaddr = getOperandValue(instr.operands[0]);
            uint64_t length = getOperandValue(instr.operands[1]);

            std::ofstream outfile("memory_dump.csv", std::ios::app);
            outfile << "Address,Value\n";

            // Read byte by byte
            for (uint64_t i = 0; i < length; ++i) {
                MemoryRequest req;
                req.vaddr = vaddr + i;
                req.size = 1;
                req.is_write = false;
                req.pid = 1;

                auto resp = memory_system_->access(req, cycle_count_);
                outfile << (vaddr + i) << "," << (int)resp.data[0] << "\n";
            }
            outfile.close();
            break;
        }
        case Opcode::SYNC: {
            // Barrier: Wait for DMA completion
            cycle_count_ += 1; // Dispatch
            if (dma_->isBusy(cycle_count_)) {
                cycle_count_ = dma_->getCompletionCycle();
            }
            break;
        }
        default:
            break;
    }
}

}
