#ifndef _RTT_H_
#define _RTT_H_

#include "commoninstruction.h"
#include "kd/common/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// RTT - return from interrupt
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
// Operation is the same as RTI except that it inhibits a trace trap while
// RTI permits trace trap. If new PS has T bit set, trap will occur after
// execution of first instruction after RTT.
//
class CommonInstruction::RTT : public NoOperandInstruction, public WithFactory<RTT>
{
public:
    RTT (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::RTT::RTT (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    NoOperandInstruction (cpu, cpuControl, mmu, instruction)
{}

//  If the RTT sets the T bit in the PS, the next instruction will be executed
// and then the trace trap will be processed. (Micro PDP-11 Handbook,
// pag 322). This is the normal behaviour for instruction setting the T-bit.
inline bool CommonInstruction::RTT::execute ()
{
    if (!mmu_->popWord (&cpu_->registers ()[7]) || 
            !mmu_->popWord (&cpu_->psw ()))
        return false;

    return true;
}

#endif // _RTT_H_