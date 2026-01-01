#include <gtest/gtest.h>
#include "simutpu/isa_parser.h"

namespace simutpu {

TEST(ISAParserTest, ParseBasic) {
    std::string script = R"(
        LOAD r1, 0x1000
        NOP
        HALT
    )";

    auto instrs = ISAParser::parse(script);
    ASSERT_EQ(instrs.size(), 3);

    EXPECT_EQ(instrs[0].opcode, Opcode::LOAD);
    ASSERT_EQ(instrs[0].operands.size(), 2);
    EXPECT_EQ(instrs[0].operands[0].type, Operand::Type::Register);
    EXPECT_EQ(instrs[0].operands[0].value, 1);
    EXPECT_EQ(instrs[0].operands[1].value, 0x1000);

    EXPECT_EQ(instrs[1].opcode, Opcode::NOP);
    EXPECT_EQ(instrs[2].opcode, Opcode::HALT);
}

TEST(ISAParserTest, ParseCompute) {
    std::string line = "VEC_ADD r1, r2, r3";
    auto instr = ISAParser::parseLine(line);

    EXPECT_EQ(instr.opcode, Opcode::VEC_ADD);
    ASSERT_EQ(instr.operands.size(), 3);
    EXPECT_EQ(instr.operands[0].value, 1);
    EXPECT_EQ(instr.operands[1].value, 2);
    EXPECT_EQ(instr.operands[2].value, 3);
}

}
