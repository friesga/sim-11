#ifndef _RESET_H_
#define _RESET_H_

#include "kd11_na.h"
#include "../nooperandinstruction/nooperandinstruction.h"
#include "../cpudata.h"
#include "../cpucontrol.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// RESET - reset external bus
//
// Condition Codes: not affected
//
// Sends INIT on the BUS for 10 micro seconds. All devices on the BUS are
// reset to their state at power-up. The processor remains in an idle state
// for 90 micro seconds following issuance of INIT.
//
class KD11_NA::RESET : public NoOperandInstruction, public WithFactory<RESET>
{
public:
    RESET (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::RESET::RESET (CpuData* cpu, u16 instruction)
    :
    NoOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::RESET::execute ()
{
    cpu_->busReset ();
    return CpuData::Trap::None;
}

#endif // _RESET_H_