#include "kd11doubleoperandinstruction.h"

KD11DoubleOperandInstruction::KD11DoubleOperandInstruction (CpuData* cpu,
        CpuControl* cpuControl, MMU* mmu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, cpuControl, mmu, instruction)
{
    getOperandLocations (cpuData_->registers ());
}