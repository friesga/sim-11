#ifndef _RTI_H_
#define _RTI_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/nooperandinstruction/nooperandinstruction.h"
#include "kd/kd11_na/cpu/cpudata.h"
#include "kd/kd11_na/cpu/cpucontrol.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
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
    CpuData::Trap execute () override;
};

CommonInstruction::RTI::RTI (CpuData* cpu, u16 instruction)
    :
    NoOperandInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::RTI::execute ()
{
    if (!cpu_->popWord (&cpu_->registers ()[7]))
        return CpuData::Trap::BusError;
    if (!cpu_->popWord (&cpu_->psw ()))
        return CpuData::Trap::BusError;

    return CpuData::Trap::None;
}

#endif // _RTI_H_