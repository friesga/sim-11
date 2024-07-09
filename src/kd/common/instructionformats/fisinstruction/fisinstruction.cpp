#include "fisinstruction.h"

FISInstruction::FISInstruction (u16 instruction)
    :
    LSI11Instruction (),
    instr_ {instruction}
{}

u16 FISInstruction::getRegister ()
{
    return instr_.decoded.rn;
}

u16 FISInstruction::getOperationCode ()
{
    return instr_.decoded.opcode;
}
