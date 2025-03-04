#include "compiler.h"
#include "translator.h"
#include <Assembler/Assembler.h>

struct HashXCompiler {
    SASS::Volta::Assembler assembler;
    
    HashXCompiler() {
        assembler.SetComputeCapability(89); // Ada Lovelace (RTX 40-series)
    }
    
    std::vector<uint8_t> compile(const hashx_program& prog) {
        auto sass_program = translate_hashx_to_sass(prog);
        
        return assembler.Assemble(sass_program);
    }
}; 