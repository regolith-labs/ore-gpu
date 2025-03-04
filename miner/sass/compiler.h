#pragma once
#include <vector>
#include <cstdint>
#include <SASS/Volta/Assembler.h>

struct hashx_program;

class HashXCompiler {
private:
    SASS::Volta::Assembler assembler;

public:
    HashXCompiler();
    std::vector<uint8_t> compile(const hashx_program& prog);
}; 