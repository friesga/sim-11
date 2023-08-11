#include "fisinstruction.h"

FISInstruction::FISInstruction (CpuData* cpu, u16 instruction)
    :
    LSI11Instruction (cpu),
    instr_ {instruction},
	cpu_ {cpu}
{}

u16 FISInstruction::getRegister ()
{
    return instr_.decoded.rn;
}