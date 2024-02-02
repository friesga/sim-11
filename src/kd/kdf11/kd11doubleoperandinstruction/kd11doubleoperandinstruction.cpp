#include "kd11doubleoperandinstruction.h"

KD11DoubleOperandInstruction::KD11DoubleOperandInstruction (CpuData* cpuData,
        CpuControl* cpuControl, MMU* mmu, u16 instruction)
    :
    DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{
    getOperandLocations (cpuData_->registers ());
}