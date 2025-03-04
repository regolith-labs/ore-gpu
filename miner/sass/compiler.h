#pragma once
#include <vector>
#include <cstdint>
#include <Assembler/Assembler.h>

struct hashx_program;

class HashXCompiler {
private:
    Assembler::Assembler assembler;

public:
    HashXCompiler();
    std::vector<uint8_t> compile(const hashx_program& prog);
}; 