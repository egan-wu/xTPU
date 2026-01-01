#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>
#include <optional>

namespace simutpu {

class MemoryDevice {
public:
    virtual ~MemoryDevice() = default;

    virtual std::string name() const = 0;
    virtual size_t size() const = 0;
    virtual int latency() const = 0;

    // Reads from physical address
    virtual std::vector<uint8_t> read(uint64_t offset, size_t length) = 0;

    // Writes to physical address
    virtual void write(uint64_t offset, const std::vector<uint8_t>& data) = 0;
};

// Simple byte-array memory
class SimpleMemory : public MemoryDevice {
public:
    SimpleMemory(std::string name, size_t size, int latency)
        : name_(std::move(name)), size_(size), latency_(latency) {
        data_.resize(size, 0);
    }

    std::string name() const override { return name_; }
    size_t size() const override { return size_; }
    int latency() const override { return latency_; }

    std::vector<uint8_t> read(uint64_t offset, size_t length) override {
        if (offset + length > size_) {
            throw std::out_of_range(name_ + ": Read out of bounds");
        }
        std::vector<uint8_t> result(length);
        std::copy(data_.begin() + offset, data_.begin() + offset + length, result.begin());
        return result;
    }

    void write(uint64_t offset, const std::vector<uint8_t>& data) override {
        if (offset + data.size() > size_) {
            throw std::out_of_range(name_ + ": Write out of bounds");
        }
        std::copy(data.begin(), data.end(), data_.begin() + offset);
    }

private:
    std::string name_;
    size_t size_;
    int latency_;
    std::vector<uint8_t> data_;
};

}
