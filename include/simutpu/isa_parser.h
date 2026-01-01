#pragma once
#include "simutpu/isa.h"
#include <string>
#include <vector>

namespace simutpu {

class ISAParser {
public:
    static std::vector<Instruction> parse(const std::string& script);
    static Instruction parseLine(const std::string& line);
};

}
