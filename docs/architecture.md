# SimuTPU Architecture

The SimuTPU is a behavioral model of a Tensor Processing Unit, focusing on the Matrix Multiply Unit (MXU).

```mermaid
graph TD
    subgraph Host
        UserApp[User Application / Test]
    end

    subgraph SimuTPU [Simulator Device]
        direction TB
        ControlUnit[Control Unit / Dispatcher]

        subgraph Compute
            MXU[Matrix Multiply Unit (Systolic Array)]
        end

        subgraph Storage
            SRAM[Memory / SRAM]
        end
    end

    UserApp -- Loads Program & Data --> SRAM
    UserApp -- Starts --> ControlUnit

    ControlUnit -- Fetches Instr --> SRAM
    ControlUnit -- Dispatches GEMM --> MXU

    MXU -- Reads A & B --> SRAM
    MXU -- Writes Result C --> SRAM

    classDef memory fill:#f9f,stroke:#333,stroke-width:2px;
    classDef compute fill:#9f9,stroke:#333,stroke-width:2px;
    classDef control fill:#99f,stroke:#333,stroke-width:2px;

    class SRAM memory;
    class MXU compute;
    class ControlUnit control;
```

## Description

1.  **Memory**: Acts as the unified storage (SRAM/HBM abstraction) containing both data (matrices) and potentially instructions (though currently instructions are separate for simplicity).
2.  **Control Unit**: Iterates through the instruction list. It decodes the opcode and routes the instruction to the appropriate unit.
3.  **MXU (Matrix Multiply Unit)**: The core compute engine. It reads input matrices A and B from the Memory, performs the matrix multiplication, and writes the result C back to Memory.
