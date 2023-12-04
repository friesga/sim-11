#include "sobinstruction.h"

SobInstruction::SobInstruction (CpuData* cpu, CpuControl* cpuControl,
		MMU* mmu, u16 instruction)
	:
	LSI11Instruction (cpu, cpuControl, mmu),
	instr_ {instruction}
{}

u16 SobInstruction::getRegisterNr ()
{
	return instr_.decoded.rn;
}

u16 SobInstruction::getOffset ()
{
	return instr_.decoded.offset;
}

// Return the instruction's operation code
u16 SobInstruction::getOperationCode ()
{
	return instr_.decoded.opcode;
}