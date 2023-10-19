#include "markinstruction.h"

MarkInstruction::MarkInstruction (CpuData* cpu, u16 instruction)
    :
    LSI11Instruction (cpu),
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