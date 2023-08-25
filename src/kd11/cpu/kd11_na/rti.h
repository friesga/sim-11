#ifndef _RTI_H_
#define _RTI_H_

#include "kd11_na.h"
#include "../nooperandinstruction/nooperandinstruction.h"
#include "../cpudata.h"
#include "../cpucontrol.h"
#include "../operandlocation/operandlocation.h"
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
class KD11_NA::RTI : public NoOperandInstruction, public WithFactory<RTI>
{
public:
    RTI (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::RTI::RTI (CpuData* cpu, u16 instruction)
    :
    NoOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::RTI::execute ()
{
    if (!cpu_->popWord (&cpu_->registers ()[7]))
        return CpuData::Trap::BusError;
    if (!cpu_->popWord (&cpu_->psw ()))
        return CpuData::Trap::BusError;

    return CpuData::Trap::None;
}

#endif // _RTI_H_