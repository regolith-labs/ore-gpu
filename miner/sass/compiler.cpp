#include "compiler.h"
#include "translator.h"
#include <Assembler/Assembler.h>
#include <Assembler/ELFGenerator.h>

HashXCompiler::HashXCompiler() : assembler() {
}

std::vector<uint8_t> HashXCompiler::compile(const hashx_program& prog) {
    auto sass_program = translate_hashx_to_sass(prog);
    
    auto binary = assembler.Assemble(sass_program);
    
    // Generate ELF binary
    Assembler::ELFGenerator elfGenerator;
    auto elf = elfGenerator.Generate(binary);
    
    // Copy to vector
    auto data = static_cast<uint8_t*>(elf->GetData());
    std::vector<uint8_t> elfBytes(data, data + elf->GetSize());
    
    delete elf;  // Cleanup allocated ELF binary
    return elfBytes;
} 