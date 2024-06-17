#include "fisinstruction.h"

FISInstruction::FISInstruction (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction)
    :
    LSI11Instruction (cpuData, cpuControl, mmu),
    instr_ {instruction}
{}

u16 FISInstruction::getRegister ()
{
    return instr_.decoded.rn;
}

u16 FISInstruction::getOperationCode ()
{
    return instr_.decoded.opcode;
}
