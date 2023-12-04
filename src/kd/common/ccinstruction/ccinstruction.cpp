#include "ccinstruction.h"

CcInstruction::CcInstruction (CpuData *cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    LSI11Instruction (cpu, cpuControl, mmu),
    instr_ {instruction}
{}

u16 CcInstruction::getConditionCodes ()
{
    return instr_.decoded.conditionCodes;
}

u16 CcInstruction::getOperationCode ()
{
    return instr_.decoded.opcode;
}