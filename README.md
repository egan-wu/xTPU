# Project Name: **SimuTPU – Configurable Behavioral NPU Model**

---

## 1. Project Overview

**Goal**  
Develop a behavior-accurate software model of a Tensor Processing Unit (TPU/NPU).

**Nature**  
- C++ library  
- Designed as a 3rd-party submodule (Git submodule–friendly)

**Purpose**  
- Simulate hardware behavior, latency, and memory traffic  
- Enable architectural exploration and software stack verification

**Simulation Level**  
- Behavioral & Cycle-Approximate  
- Models data-movement latency and compute throughput  
- Does *not* simulate RTL-level signal toggling

---

## 2. Technical Specifications

- **Language**: C++20  
  - Utilizes concepts, ranges, and smart pointers
- **Build System**: CMake  
  - Modern target-based approach  
  - Supports integration via `add_subdirectory`
- **Dependency Policy**: Minimal  
  - Configuration via `nlohmann/json` or `yaml-cpp`  
  - No heavy frameworks
- **Coding Style**:  
  - Google C++ Style Guide  
  - Enforced with `clang-format`

---

## 3. System Architecture

### Top-Level Module (Core)
- Manages:
  - Simulation loop
  - State machine
  - Coordination of sub-components

### Memory Hierarchy

- **Host Memory**
  - Simulated DRAM accessible by the host
- **Device Memory (HBM)**
  - High-bandwidth memory model on device
- **On-Chip Buffer (SRAM)**
  - Global buffer for staging data
- **Registers**
  - Scalar registers  
  - Vector registers

### Compute Units

- **Matrix Multiply Unit (MXU)**
  - Systolic array simulation
  - Supports:
    - Output-stationary
    - Weight-stationary dataflow
- **Vector Processing Unit (VPU)**
  - Activation functions (ReLU, GeLU)
  - Element-wise operations

### Interconnect

- Simulates:
  - Bus latency
  - Bandwidth constraints
- Applies to communication between memory levels

---

## 4. Functional Requirements

### Adjustable Configuration (Runtime / Compile-Time)

- Systolic array size  
  - Examples: `16×16`, `128×128`
- Memory parameters  
  - SRAM size  
  - HBM bandwidth
- Latency parameters  
  - DRAM access latency  
  - ALU latency

### Data Type Support

- Abstract `DataType` class supporting:
  - FP32
  - FP16
  - BF16
  - INT8
  - INT4

### Scalability

- **Scale-Up**
  - Multiple `Core` instances within a single `Device`
- **Scale-Out**
  - Abstract interface for device-to-device communication
  - Simulates chip-level interconnects

---

## 5. Instruction Set Architecture (ISA) Simulation

### Instruction Stream Execution

- Parses and executes a programmable opcode stream

### Control Instructions

- `HALT`
- `NOP`
- `SYNC` (barrier)

### Data Movement Instructions

- `LOAD`  
  - Host → Device  
  - HBM → SRAM
- `STORE`
- `DMA_XFER`

### Compute Instructions

- `GEMM` – General Matrix Multiply
- `VEC_ADD` – Element-wise addition
- `VEC_MUL` – Element-wise multiplication
- `ACT` – Activation functions

### ISA Parser

- Simple decoder supporting:
  - Binary format
  - Assembly-like text format
- Translates instructions into internal command objects

---

## 6. Verification & Validation Strategy

### Unit Testing (GoogleTest)

- Verify individual modules independently:
  - Systolic array logic
  - Memory Management Unit (MMU) address translation

### Golden Model Comparison

- **Python / NumPy Reference**
  - Reference implementation for matrix operations
- **Compliance Testing**
  - C++ model exports results (CSV or binary)
  - Python verification script compares outputs
  - Supports:
    - Bit-exact comparison
    - Tolerance-based comparison

### Trace & Logging

- **Instruction Trace**
  - Logs executed instruction
  - Records Program Counter (PC)
- **Memory Trace**
  - Logs all memory read/write addresses
  - Used to visualize access patterns
- **Performance Counters**
  - Total cycle count
  - Stall cycles
  - ALU utilization rate

### Sanity Check

- Embedded “Hello World” GEMM test
  - Example: `2×2` matrix multiplication
- Integrated into CI pipeline

---

## 7. Integration Interface

### Input

- `Config` object  
  - Hardware specifications
- `InstructionBuffer`  
  - Program instructions
- `DataBuffer`  
  - Initial memory state

### Output

- Final memory state
- Cycle count report