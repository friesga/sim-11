#include "markinstruction.h"

MarkInstruction::MarkInstruction (KD11CPU *cpu, u16 instruction)
    :
    cpu_ {cpu},
    instr_ {instruction}
{}

u16 MarkInstruction::numberOfParameters ()
{
    return instr_.decoded.nn;
}