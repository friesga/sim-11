#ifndef _EMT_H_
#define _EMT_H_

#include "commoninstruction.h"
#include "kd/common/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// EMT - emulator trap
//
// Operation:
//  v(SP) <- PS
//  v(SP) <- PC
//  PC <- (30)
//  PS <- (32)
//
// Condition Codes:
//  N: loaded from trap vector
//  Z: loaded from trap vector
//  V: loaded from trap vector
//  C: loaded from trap vector
//
// All operation codes from 104000 to 104377 are EMT instructions and may be
// used to transmit information to the emulating routine (e.g., function to be
// performed). The trap vector for EMT is at address 30. The new PC is taken
// from the word at address 30; the new processor status (PS) is taken from the
// word at address 32.
//
class CommonInstruction::EMT : public NoOperandInstruction, public WithFactory<EMT>
{
public:
    EMT (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::EMT::EMT (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    NoOperandInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::EMT::execute ()
{
    cpu_->setTrap (CpuData::TrapCondition::EmulatorTrap);
    return true;
}

#endif // _EMT_H_