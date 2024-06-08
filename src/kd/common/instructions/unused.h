#ifndef _UNUSED_H_
#define _UNUSED_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "trace/trace.h"


// Unused/reserved instruction
//
// Reserved instructions trap to vector address 10.
//
class CommonInstruction::Unused : public NoOperandInstruction
{
public:
    Unused (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

// The two function members are defined inline as this header file is
// included in both opcodetables.cpp and kd11_na.cpp.
inline CommonInstruction::Unused::Unused (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    NoOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::Unused::execute ()
{
    throw "Should not happen";
}

#endif // _UNUSED_H_