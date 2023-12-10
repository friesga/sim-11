#ifndef _UNUSED_H_
#define _UNUSED_H_

#include "commoninstruction.h"
#include "kd/common/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "trace/trace.h"
#include "withfactory.h"

// Unused/reserved instruction
//
// Reserved instructions trap to vector address 10.
//
class CommonInstruction::Unused : public NoOperandInstruction, public WithFactory<Unused>
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

inline bool CommonInstruction::Unused::execute ()
{
    cpuData_->setTrap (CpuData::TrapCondition::ReservedInstructionTrap);
    return true;
}

#endif // _UNUSED_H_