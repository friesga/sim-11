#ifndef _RESET_H_
#define _RESET_H_

#include "kd/common/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// RESET - reset external bus
//
// Condition Codes: not affected
//
// Sends INIT on the BUS for 10 micro seconds. All devices on the BUS are
// reset to their state at power-up. The processor remains in an idle state
// for 90 micro seconds following issuance of INIT.
//
// A RESET executed in user mode results in a NOP (EK-KDF11-UG-PR2,
// par. 8.3.3.3).
//
class KDF11Instruction::RESET : public NoOperandInstruction, public WithFactory<RESET>
{
public:
    RESET (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11Instruction::RESET::RESET (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    NoOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11Instruction::RESET::execute ()
{
    if (cpuData_->psw ().currentMode () != PSW::Mode::User)
        cpuControl_->busReset ();

    return true;
}

#endif // _RESET_H_