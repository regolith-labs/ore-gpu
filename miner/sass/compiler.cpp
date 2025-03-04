#include "compiler.h"
#include "translator.h"
#include <Assembler/Assembler.h>

HashXCompiler::HashXCompiler() : assembler() {
    assembler.SetComputeCapability(89); // Explicitly set Ada CC 8.9
}

std::vector<uint8_t> HashXCompiler::compile(const hashx_program& prog) {
    auto sass_program = translate_hashx_to_sass(prog);
    
    return assembler.Assemble(sass_program);
} 