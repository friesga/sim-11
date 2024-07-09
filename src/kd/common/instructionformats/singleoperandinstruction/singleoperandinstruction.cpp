#include "singleoperandinstruction.h"
#include "kd/common/operand/operand.h"

// Initialize the instr struct with the first member
SingleOperandInstruction::SingleOperandInstruction (CpuData* cpuData, CpuControl* cpuControl,
		MMU* mmu, u16 instruction)
	:
	LSI11Instruction (cpuData, cpuControl, mmu),
	instr_ {instruction}
{}

u16 SingleOperandInstruction::getRegister ()
{
	return instr_.decoded.rn;
}

u16 SingleOperandInstruction::getMode ()
{
	return instr_.decoded.mode;
}

// Return the instruction's operation code
u16 SingleOperandInstruction::getOperationCode ()
{
	return instr_.decoded.opcode;
}

