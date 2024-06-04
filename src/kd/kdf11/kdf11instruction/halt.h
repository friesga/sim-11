#ifndef _HALT_H_
#define _HALT_H_

#include "kd/common/instructionformats/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// HALT - halt
// 
// Condition Codes: not affected
// 
// Causes the processor to leave RUN mode. The PC points to the next
// instruction to be executed. The processor goes into the HALT mode.
// The console mode of operation is enabled.
//
// A HALT in user mode traps to location 10 (EK-KDF11-UG-PR2, par. 8.3.3.3).
//
class KDF11Instruction::HALT : public NoOperandInstruction, public WithFactory<HALT>
{
public:
    HALT (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11Instruction::HALT::HALT (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    NoOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11Instruction::HALT::execute ()
{
    if (cpuData_->psw ().currentMode () == PSW::Mode::User)
    {
        cpuData_->setTrap (CpuData::TrapCondition::ReservedInstructionTrap);
        return false;
    }
    cpuControl_->halt ();
    return true;
}

#endif // _HALT_H_