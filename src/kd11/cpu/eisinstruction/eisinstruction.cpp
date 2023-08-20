#include "eisinstruction.h"
#include "../operand/operand.h"

EisInstruction::EisInstruction (CpuData* cpu, u16 instruction)
    :
	LSI11Instruction (cpu),
    instr_ {instruction}
{}

// Derive the location of the operand from the instruction
OperandLocation EisInstruction::getOperandLocation (u16 (&reg)[8])
{
	return decodeOperand (Operand {instr_.decoded.operandRegNr,
		instr_.decoded.operandMode}, reg);
}

// The operand in the EIS instruction format is either a source or
// a destination operand.
bool EisInstruction::readOperand (CondData<u16> *source)
{
	OperandLocation sourceOperandLocation = 
		getOperandLocation (cpu_->registers ());
    *source = sourceOperandLocation.wordContents ();
	return (*source).hasValue ();
}

// Return the instruction's operation code
u16 EisInstruction::getOperationCode ()
{
	return instr_.decoded.opcode;
}

u16 EisInstruction::getRegisterNr ()
{
	return instr_.decoded.registerNr;
}