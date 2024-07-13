#include "eisinstruction.h"
#include "kd/common/operand/operand.h"

EisInstruction::EisInstruction (u16 instruction)
    :
    instr_ {instruction}
{}

// Return the instruction's operation code
u16 EisInstruction::getOperationCode () const
{
	return instr_.decoded.opcode;
}

u16 EisInstruction::getOperandRegister () const
{
	return instr_.decoded.operandRegNr;
}

u16 EisInstruction::getOperandMode () const
{
	return instr_.decoded.operandMode;
}

u16 EisInstruction::getRegisterNr () const
{
	return instr_.decoded.registerNr;
}

// The following two functions are specific for use for the ASH and ASHC
// instructions. The source operand of these instructions is a shift count.
// This count is a 2's complement number and ranges from -32 to +31.
// Negative is a right shift and positive is a left shift
int EisInstruction::getShiftCount () const
{
    return sixBitTwoComplementToInt (instr_.word);
}

s8 EisInstruction::sixBitTwoComplementToInt (u16 instruction) const
{
    s8 signed8bit = instruction & sourceOperandMask;

    if (signed8bit & signBit)
        // Negative number, extend the sign bit to the full eight bits
        signed8bit |= ~sourceOperandMask;

    return signed8bit;
}

