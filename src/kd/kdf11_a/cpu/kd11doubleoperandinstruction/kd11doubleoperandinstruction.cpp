#include "kd11doubleoperandinstruction.h"

KD11DoubleOperandInstruction::KD11DoubleOperandInstruction (CpuData* cpu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, instruction)
{
    getOperandLocations (cpu_->registers ());
}