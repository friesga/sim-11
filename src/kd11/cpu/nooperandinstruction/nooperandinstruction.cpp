#include "nooperandinstruction.h"

NoOperandInstruction::NoOperandInstruction (CpuData* cpu, u16 instruction)
    :
    LSI11Instruction (cpu),
    instr_ {instruction}
{}

// Return the instruction's operation code. The operation code takes up
// the complete instruction.
u16 NoOperandInstruction::getOperationCode ()
{
	return instr_;
}