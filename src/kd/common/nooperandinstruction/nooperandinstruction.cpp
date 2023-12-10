#include "nooperandinstruction.h"

NoOperandInstruction::NoOperandInstruction (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction)
    :
    LSI11Instruction (cpuData, cpuControl, mmu),
    instr_ {instruction}
{}

// Return the instruction's operation code. The operation code takes up
// the complete instruction.
u16 NoOperandInstruction::getOperationCode ()
{
	return instr_;
}