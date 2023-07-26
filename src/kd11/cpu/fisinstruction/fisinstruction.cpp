#include "fisinstruction.h"

FISInstruction::FISInstruction (KD11CPU *cpu, u16 instruction)
    :
    instr_ {instruction},
	cpu_ {cpu}
{}

u16 FISInstruction::getRegister ()
{
    return instr_.decoded.rn;
}