#include "nooperandinstruction.h"

NoOperandInstruction::NoOperandInstruction (u16 instruction)
    :
    PDP11Instruction (),
    instr_ {instruction}
{}

// Return the instruction's operation code. The operation code takes up
// the complete instruction.
u16 NoOperandInstruction::getOperationCode () const
{
	return instr_;
}