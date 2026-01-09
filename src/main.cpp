#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "simutpu/core.h"
#include "simutpu/isa_parser.h"
#include "simutpu/config.h"
#include "simutpu/tracer.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <program.asm> [config.json]" << std::endl;
        return 1;
    }

    std::string programFile = argv[1];
    std::string configFile = (argc >= 3) ? argv[2] : "";

    // Open Trace
    simutpu::Tracer::getInstance().open("sim_trace.json");

    try {
        // Load Config
        simutpu::Config config;
        if (!configFile.empty()) {
            config = simutpu::Config::load(configFile);
        }

        // Read Program
        std::ifstream file(programFile);
        if (!file) {
            std::cerr << "Error: Could not open program file " << programFile << std::endl;
            return 1;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string script = buffer.str();

        // Parse
        auto instructions = simutpu::ISAParser::parse(script);

        // Run
        simutpu::Core core(config);

        // Hack: Map a default page so basic loads/stores work for 0x0 - 0x200000 range
        // In a real OS this is done by loader.
        core.getMemorySystem().getMMU().mapPage(1, 0, 0, simutpu::PageSize::Size2MB, true, false);

        core.run(instructions);

        std::cout << "Simulation finished." << std::endl;
        std::cout << "Cycle Count: " << core.getCycleCount() << std::endl;
        std::cout << "Registers:" << std::endl;
        for (int i = 0; i < 8; ++i) { // Print first 8
            std::cout << "R" << i << ": 0x" << std::hex << core.getRegister(i) << std::dec << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
