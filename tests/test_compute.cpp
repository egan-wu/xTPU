#include <gtest/gtest.h>
#include "simutpu/compute.h"

namespace simutpu {

TEST(MXUTest, GEMM_FP32) {
    Config config;
    MXU mxu(config);

    Matrix<float> A = {{1.0, 2.0}, {3.0, 4.0}};
    Matrix<float> B = {{0.5, 1.0}, {1.0, 0.0}};
    Matrix<float> C;

    mxu.execute(A, B, C);

    // Expected:
    // [1*0.5 + 2*1, 1*1 + 2*0] = [2.5, 1.0]
    // [3*0.5 + 4*1, 3*1 + 4*0] = [5.5, 3.0]

    EXPECT_FLOAT_EQ(C[0][0], 2.5);
    EXPECT_FLOAT_EQ(C[0][1], 1.0);
    EXPECT_FLOAT_EQ(C[1][0], 5.5);
    EXPECT_FLOAT_EQ(C[1][1], 3.0);
}

TEST(VPUTest, VectorAdd) {
    Config config;
    VPU vpu(config);

    std::vector<float> a = {1.0, 2.0, 3.0};
    std::vector<float> b = {4.0, 5.0, 6.0};
    std::vector<float> c;

    vpu.execute(VPU::Op::ADD, a, b, c);

    EXPECT_EQ(c.size(), 3);
    EXPECT_FLOAT_EQ(c[0], 5.0);
    EXPECT_FLOAT_EQ(c[1], 7.0);
    EXPECT_FLOAT_EQ(c[2], 9.0);
}

TEST(VPUTest, ReLU) {
    Config config;
    VPU vpu(config);

    std::vector<float> a = {-1.0, 0.0, 2.0};
    std::vector<float> dummy = {0,0,0}; // Not used for unary
    std::vector<float> c;

    vpu.execute(VPU::Op::RELU, a, dummy, c);

    EXPECT_FLOAT_EQ(c[0], 0.0);
    EXPECT_FLOAT_EQ(c[1], 0.0);
    EXPECT_FLOAT_EQ(c[2], 2.0);
}

}
