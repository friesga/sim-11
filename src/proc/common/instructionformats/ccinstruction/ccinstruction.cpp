#include "ccinstruction.h"

CcInstruction::CcInstruction (u16 instruction)
    :
    instr_ {instruction}
{}

u16 CcInstruction::getConditionCodes () const
{
    return instr_.decoded.conditionCodes;
}

u16 CcInstruction::getOperationCode () const
{
    return instr_.decoded.opcode;
}