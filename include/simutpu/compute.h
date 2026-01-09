#pragma once
#include <vector>
#include <cstdint>
#include <functional>
#include "simutpu/types.h"
#include "simutpu/config.h"

namespace simutpu {

class ComputeUnit {
public:
    virtual ~ComputeUnit() = default;

    // Returns latency in cycles
    virtual int latency() const = 0;
};

// Abstract matrix storage for simulation
template <typename T>
using Matrix = std::vector<std::vector<T>>;

class MXU : public ComputeUnit {
public:
    explicit MXU(const Config& config);

    int latency() const override;

    // Perform GEMM: C = A * B
    // A: MxK, B: KxN, C: MxN
    // Simplification: We assume data is already loaded into internal buffers or passed directly.
    // In a real simulator, this would operate on Memory System addresses or internal registers.
    // For behavioral simulation, passing data directly is easier to verify logic.
    template <typename T>
    void execute(const Matrix<T>& A, const Matrix<T>& B, Matrix<T>& C) {
        size_t M = A.size();
        size_t K = A[0].size();
        size_t N = B[0].size();

        // Resize C if needed
        if (C.size() != M || C[0].size() != N) {
            C.resize(M, std::vector<T>(N, 0));
        }

        // Naive O(N^3) implementation for behavioral correctness
        // Systolic array timing is modeled by latency()
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                T sum = 0;
                for (size_t k = 0; k < K; ++k) {
                    sum += A[i][k] * B[k][j];
                }
                C[i][j] = sum;
            }
        }
    }

private:
    Config config_;
};

class VPU : public ComputeUnit {
public:
    explicit VPU(const Config& config);

    int latency() const override;

    enum class Op {
        ADD,
        MUL,
        RELU,
        GELU
    };

    template <typename T>
    void execute(Op op, const std::vector<T>& src1, const std::vector<T>& src2, std::vector<T>& dst) {
        size_t n = src1.size();
        dst.resize(n);

        for (size_t i = 0; i < n; ++i) {
            switch (op) {
                case Op::ADD:
                    dst[i] = src1[i] + src2[i];
                    break;
                case Op::MUL:
                    dst[i] = src1[i] * src2[i];
                    break;
                case Op::RELU:
                    dst[i] = (src1[i] > 0) ? src1[i] : 0;
                    break;
                default:
                    break;
            }
        }
    }

private:
    Config config_;
};

}
