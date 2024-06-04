#include "markinstruction.h"

MarkInstruction::MarkInstruction (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction)
    :
    LSI11Instruction (cpuData, cpuControl, mmu),
    instr_ {instruction}
{}

u16 MarkInstruction::numberOfParameters ()
{
    return instr_.decoded.nn;
}

// Return the instruction's operation code
u16 MarkInstruction::getOperationCode ()
{
	return instr_.decoded.opcode;
}