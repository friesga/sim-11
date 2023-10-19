#ifndef _RESET_H_
#define _RESET_H_

#include "commoninstruction.h"
#include "kd/common/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// RESET - reset external bus
//
// Condition Codes: not affected
//
// Sends INIT on the BUS for 10 micro seconds. All devices on the BUS are
// reset to their state at power-up. The processor remains in an idle state
// for 90 micro seconds following issuance of INIT.
//
class CommonInstruction::RESET : public NoOperandInstruction, public WithFactory<RESET>
{
public:
    RESET (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::RESET::RESET (CpuData* cpu, u16 instruction)
    :
    NoOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::RESET::execute ()
{
    cpu_->busReset ();
    return CpuData::Trap::None;
}

#endif // _RESET_H_