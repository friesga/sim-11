#include "eisinstruction.h"
#include "kd/common/operand/operand.h"

EisInstruction::EisInstruction (CpuData* cpuData, CpuControl* cpuControl,
		MMU* mmu, u16 instruction)
    :
	LSI11Instruction (cpuData, cpuControl, mmu),
    instr_ {instruction}
{}

// Return the instruction's operation code
u16 EisInstruction::getOperationCode ()
{
	return instr_.decoded.opcode;
}

u16 EisInstruction::getOperandRegister ()
{
	return instr_.decoded.operandRegNr;
}

u16 EisInstruction::getOperandMode ()
{
	return instr_.decoded.operandMode;
}

u16 EisInstruction::getRegisterNr ()
{
	return instr_.decoded.registerNr;
}

