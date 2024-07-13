#include "fisinstruction.h"

FISInstruction::FISInstruction (u16 instruction)
    :
    PDP11Instruction (),
    instr_ {instruction}
{}

u16 FISInstruction::getRegister () const
{
    return instr_.decoded.rn;
}

u16 FISInstruction::getOperationCode () const
{
    return instr_.decoded.opcode;
}
