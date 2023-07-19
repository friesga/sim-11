#include "ccinstruction.h"

CcInstruction::CcInstruction (u16 instruction)
    :
    instr_ {instruction}
{}

u16 CcInstruction::getConditionCodes ()
{
    return instr_.decoded.conditionCodes;
}