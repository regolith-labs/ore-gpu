#include "compiler.h"
#include "translator.h"
#include <SASS/Volta/Assembler.h>

HashXCompiler::HashXCompiler() : assembler() {
    assembler.SetComputeCapability(70); // Volta (CC 7.0)
}

std::vector<uint8_t> HashXCompiler::compile(const hashx_program& prog) {
    auto sass_program = translate_hashx_to_sass(prog);
    
    return assembler.Assemble(sass_program);
} 