#include "markinstruction.h"

MarkInstruction::MarkInstruction (CpuData* cpu, u16 instruction)
    :
    cpu_ {cpu},
    instr_ {instruction}
{}

u16 MarkInstruction::numberOfParameters ()
{
    return instr_.decoded.nn;
}