#include "eisinstruction.h"
#include "../operand/operand.h"

EisInstruction::EisInstruction (CpuData* cpu, u16 instruction)
    :
	LSI11Instruction (cpu),
    instr_ {instruction},
    cpu_ {cpu}
{}

// Derive the location of the operand from the instruction
OperandLocation EisInstruction::getOperandLocation (u16 (&reg)[8])
{
	return decodeOperand (Operand {instr_.decoded.operandRegNr,
		instr_.decoded.operandMode}, reg);
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