#include "simutpu/memory.h"
#include <iostream>

namespace simutpu {

Memory::Memory(size_t size_bytes) {
    // Assuming DataType is aligned, size is number of elements
    size_t num_elements = size_bytes / sizeof(DataType);
    storage_.resize(num_elements, 0.0f);
}

DataType Memory::read(Address addr) const {
    if (addr >= storage_.size()) {
        throw std::out_of_range("Memory read out of bounds");
    }
    return storage_[addr];
}

void Memory::write(Address addr, DataType value) {
    if (addr >= storage_.size()) {
        throw std::out_of_range("Memory write out of bounds");
    }
    storage_[addr] = value;
}

std::vector<DataType> Memory::readBlock(Address addr, size_t count) const {
    if (addr + count > storage_.size()) {
        throw std::out_of_range("Memory block read out of bounds");
    }
    std::vector<DataType> result(count);
    for (size_t i = 0; i < count; ++i) {
        result[i] = storage_[addr + i];
    }
    return result;
}

void Memory::writeBlock(Address addr, const std::vector<DataType>& data) {
    if (addr + data.size() > storage_.size()) {
        throw std::out_of_range("Memory block write out of bounds");
    }
    for (size_t i = 0; i < data.size(); ++i) {
        storage_[addr + i] = data[i];
    }
}

} // namespace simutpu
