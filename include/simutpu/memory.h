#ifndef SIMUTPU_MEMORY_H
#define SIMUTPU_MEMORY_H

#include "simutpu/types.h"
#include <vector>
#include <stdexcept>

namespace simutpu {

class Memory {
public:
    Memory(size_t size_bytes);

    // Read a single value
    DataType read(Address addr) const;

    // Write a single value
    void write(Address addr, DataType value);

    // Helper to read a block (e.g. for matrix loading)
    std::vector<DataType> readBlock(Address addr, size_t count) const;

    // Helper to write a block
    void writeBlock(Address addr, const std::vector<DataType>& data);

private:
    std::vector<DataType> storage_;
};

} // namespace simutpu

#endif // SIMUTPU_MEMORY_H
