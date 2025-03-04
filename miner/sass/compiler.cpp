#include "compiler.h"
#include "translator.h"
#include <Assembler/Assembler.h>

HashXCompiler::HashXCompiler() : assembler() {
}

std::vector<uint8_t> HashXCompiler::compile(const hashx_program& prog) {
    auto sass_program = translate_hashx_to_sass(prog);
    
    auto binary = assembler.Assemble(sass_program);
    return binary->GetBytes();
} 