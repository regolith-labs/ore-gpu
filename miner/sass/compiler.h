#pragma once
#include <vector>
#include <cstdint>

struct hashx_program;

class HashXCompiler {
public:
    HashXCompiler();
    std::vector<uint8_t> compile(const hashx_program& prog);
}; 