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
    RTI (CpuData* cpu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::RTI::RTI (CpuData* cpu, u16 instruction)
    :
    NoOperandInstruction (cpu, instruction)
{}

inline bool CommonInstruction::RTI::execute ()
{
    if (!cpu_->popWord (&cpu_->registers ()[7]))
        return false;
    if (!cpu_->popWord (&cpu_->psw ()))
        return false;

    return true;
}

#endif // _RTI_H_