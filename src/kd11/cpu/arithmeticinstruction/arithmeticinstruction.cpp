#include "arithmeticinstruction.h"

ArithmeticInstruction::ArithmeticInstruction (KD11CPU *cpu, u16 instruction)
    :
    instr_ {instruction},
	cpu_ {cpu}
{}

u16 ArithmeticInstruction::getRegister ()
{
    return instr_.decoded.rn;
}