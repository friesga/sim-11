#include "ccinstruction.h"

CcInstruction::CcInstruction (u16 instruction)
    :
    LSI11Instruction (),
    instr_ {instruction}
{}

u16 CcInstruction::getConditionCodes ()
{
    return instr_.decoded.conditionCodes;
}

u16 CcInstruction::getOperationCode ()
{
    return instr_.decoded.opcode;
}