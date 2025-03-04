#include <SASS/Tree/Tree.h>
#include <SASS/Volta/Volta.h>
#include <SASS/Volta/IMULInstruction.h>

using namespace SASS::Volta;

SASS::Program* translate_hashx_to_sass(const hashx_program& prog) {
    auto program = new SASS::Program();
    program->SetComputeCapability(75); // Match target GPU
    
    auto function = new SASS::Function("hashx_sass");
    function->SetRegisters(8); // Use 8 registers
    
    auto block = new SASS::BasicBlock("BB0");
    function->AddBasicBlock(block);

    // Map HashX registers to SASS registers
    std::array<SASS::Register*, 8> regs;
    for(int i = 0; i < 8; ++i) {
        regs[i] = new SASS::Register(i);
    }

    for(const auto& instr : prog.code) {
        switch(instr.opcode) {
        case INSTR_ADD_RS:
            block->AddInstruction(new SASS::Volta::IADDInstruction_VOLTA(
                regs[instr.dst], 
                regs[instr.src],
                new SASS::Constant(instr.imm32),
                SASS::RZ
            ));
            break;
        case INSTR_SMULH_R:
            block->AddInstruction(new SASS::Volta::IMULInstruction(
                regs[instr.dst],
                regs[instr.src],
                SASS::Volta::IMULInstruction::Mode::HI
            ));
            break;
        case INSTR_UMULH_R:
            block->AddInstruction(new SASS::Volta::IMULInstruction(
                regs[instr.dst],
                regs[instr.src],
                SASS::Volta::IMULInstruction::Mode::HI
            ));
            break;
        case INSTR_MUL_R:
            block->AddInstruction(new SASS::Volta::IMULInstruction(
                regs[instr.dst],
                regs[instr.src],
                SASS::Volta::IMULInstruction::Mode::HI
            ));
            break;
        case INSTR_SUB_R:
            block->AddInstruction(new SASS::Volta::IADDInstruction_VOLTA(
                regs[instr.dst], 
                regs[instr.src],
                new SASS::Constant(-instr.imm32),
                SASS::RZ
            ));
            break;
        case INSTR_XOR_R:
            block->AddInstruction(new SASS::Volta::XORInstruction(
                regs[instr.dst],
                regs[instr.src],
                new SASS::Constant(instr.imm32)
            ));
            break;
        case INSTR_ROR_C:
            block->AddInstruction(new SASS::Volta::RORInstruction(
                regs[instr.dst],
                regs[instr.src],
                new SASS::Constant(instr.imm32)
            ));
            break;
        case INSTR_ADD_C:
            block->AddInstruction(new SASS::Volta::IADDInstruction_VOLTA(
                regs[instr.dst], 
                regs[instr.src],
                new SASS::Constant(instr.imm32),
                SASS::RZ
            ));
            break;
        case INSTR_XOR_C:
            block->AddInstruction(new SASS::Volta::XORInstruction(
                regs[instr.dst],
                regs[instr.src],
                new SASS::Constant(instr.imm32)
            ));
            break;
        case INSTR_TARGET:
            // No operands needed
            continue;
        case INSTR_BRANCH: {
            if (!current_target) {
                throw std::runtime_error("BRANCH without TARGET");
            }
            
            // AND operation: temp = src & imm32
            SASS::Register* temp = new SASS::Register(8);
            SASS::Constant* mask = new SASS::Constant(instr.imm32);
            block->AddInstruction(new SASS::Volta::ANDInstruction(
                temp,
                regs[instr.src],
                mask
            ));
            
            // Compare with zero
            SASS::Register* zero = new SASS::Register(9);
            block->AddInstruction(new SASS::Volta::CMPInstruction(
                zero,
                temp,
                SASS::RZ
            ));
            
            // Conditional branch
            SASS::Register* cmp = new SASS::Register(10);
            block->AddInstruction(new SASS::Volta::BRAInstruction(
                cmp,
                current_target
            ));
            
            current_target = nullptr;
            continue;
        }
        default:
            throw std::runtime_error("Unknown instruction opcode");
        }
    }

    program->AddFunction(function);
    return program;
} 