#ifndef _HALT_H_
#define _HALT_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// HALT - halt
// 
// Condition Codes: not affected
// 
// Causes the processor to leave RUN mode. The PC points to the next
// instruction to be executed. The processor goes into the HALT mode.
// The console mode of operation is enabled.
//
class CommonInstruction::HALT : public NoOperandInstruction, public WithFactory<HALT>
{
public:
    HALT (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::HALT::HALT (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    NoOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::HALT::execute ()
{
    cpuControl_->halt ();
    return true;
}

#endif // _HALT_H_