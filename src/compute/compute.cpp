#include "simutpu/compute.h"

namespace simutpu {

MXU::MXU(const Config& config) : config_(config) {}

int MXU::latency() const {
    // Systolic array latency model
    // E.g., Width + Height + K (pipeline fill + drain)
    // For now, return a constant from config or simple calculation
    return config_.mxu_height + config_.mxu_width;
}

VPU::VPU(const Config& config) : config_(config) {}

int VPU::latency() const {
    return config_.alu_latency;
}

}
