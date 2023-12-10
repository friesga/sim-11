#ifndef _RTI_H_
#define _RTI_H_

#include "commoninstruction.h"
#include "kd/common/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// RTI - return from interrupt
//
// Operation:
//  PC <- (SP)^
//  PS <- (SP)^
//
// Conditions Codes:
//  N: loaded from processor stack
//  Z: loaded from processor stack
//  V: loaded from processor stack
//  C: loaded from processor stack
//
// Used to exit from an interrupt or TRAP service routine. The PC and PS are
// restored (popped) from-the processor stack. If a trace trap is pending, the
// first instruction after RTI will not be executed prior to the next T traps.
//
class CommonInstruction::RTI : public NoOperandInstruction, public WithFactory<RTI>
{
public:
    RTI (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::RTI::RTI (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    NoOperandInstruction (cpu, cpuControl, mmu, instruction)
{}


// If an RTI [instruction] sets the T bit in the PS, a trace trap will occur
// before the next instruction is executed. (Micro PDP-11 Handbook, pag 322.)
// This behaviour differs from the other instructionss in which setting the
// T-bit results in a trace trap on the next instruction.
inline bool CommonInstruction::RTI::execute ()
{
    u16 tmp;

    if (!mmu_->popWord (&cpuData_->registers ()[7]) || !mmu_->popWord (&tmp))
        return false;

    cpuData_->loadPSW (tmp);
    if (cpuData_->psw () & PSW_T)
        cpuData_->setTrap (CpuData::TrapCondition::BreakpointTrap);

    return true;
}

#endif // _RTI_H_