#include <SASS/Tree/Tree.h>
#include <SASS/Utils/PrettyPrinter.h>
#include <instruction.h>
#include <program.h>

SASS::Program* translate_hashx_to_sass(const hashx_program& prog) {
    SASS::BasicBlock* current_target = nullptr;
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
            block->AddInstruction(new SASS::Volta::IADD3Instruction(
                regs[instr.dst], 
                regs[instr.src], 
                new SASS::Constant(0x0, instr.imm32),
                SASS::RZ,
                SASS::PT,
                SASS::Volta::IADD3Instruction::Flags::None
            ));
            break;
        case INSTR_SMULH_R:
            block->AddInstruction(new SASS::Volta::IMADInstruction(
                regs[instr.dst],
                regs[instr.src],
                SASS::Volta::IMADInstruction::Mode::HI
            ));
            break;
        case INSTR_UMULH_R:
            block->AddInstruction(new SASS::Volta::IMADInstruction(
                regs[instr.dst],
                regs[instr.src],
                SASS::Volta::IMADInstruction::Mode::HI
            ));
            break;
        case INSTR_MUL_R:
            block->AddInstruction(new SASS::Volta::IMADInstruction(
                regs[instr.dst],
                regs[instr.src],
                new SASS::Constant(0x0, 1)
            ));
            break;
        case INSTR_SUB_R:
            block->AddInstruction(new SASS::Volta::IADD3Instruction(
                regs[instr.dst], 
                regs[instr.src],
                new SASS::Constant(0x0, -instr.imm32),
                SASS::RZ,
                SASS::PT,
                SASS::Volta::IADD3Instruction::Flags::None
            ));
            break;
        case INSTR_XOR_R:
            block->AddInstruction(new SASS::Volta::LOP3Instruction(
                regs[instr.dst],
                regs[instr.src],
                new SASS::Constant(0x0, instr.imm32),
                SASS::Volta::LOP3Instruction::LogicOp::XOR
            ));
            break;
        case INSTR_ROR_C:
            block->AddInstruction(new SASS::Volta::SHFInstruction(
                regs[instr.dst],
                regs[instr.src],
                new SASS::Constant(0x0, instr.imm32),
                SASS::Volta::SHFInstruction::Mode::R_ROR
            ));
            break;
        case INSTR_ADD_C:
            block->AddInstruction(new SASS::Volta::IADD3Instruction(
                regs[instr.dst], 
                regs[instr.src],
                new SASS::Constant(instr.imm32),
                SASS::RZ,
                SASS::PT,
                SASS::Volta::IADD3Instruction::Flags::None
            ));
            break;
        case INSTR_XOR_C:
            block->AddInstruction(new SASS::Volta::LOP3Instruction(
                regs[instr.dst],
                regs[instr.src],
                new SASS::Constant(0x0, instr.imm32),
                SASS::Volta::LOP3Instruction::LogicOp::XOR
            ));
            break;
        case INSTR_TARGET:
            // No operands needed
            continue;
        case INSTR_BRANCH: {
            if (!current_target) {
                throw std::runtime_error("BRANCH without TARGET");
            }
            
            block->AddInstruction(new SASS::Volta::BSSYInstruction(
                new SASS::BasicBlockOperand(current_target)
            ));
            current_target = nullptr;
            break;
        }
        default:
            throw std::runtime_error("Unknown instruction opcode");
        }
    }

    program->AddFunction(function);
    return program;
} 