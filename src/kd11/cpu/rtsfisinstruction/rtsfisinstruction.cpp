#include "rtsfisinstruction.h"

RtsFisInstruction::RtsFisInstruction (KD11CPU *cpu, u16 instruction)
    :
    instr_ {instruction},
	cpu_ {cpu}
{}

u16 RtsFisInstruction::getRegister ()
{
    return instr_.decoded.rn;
}