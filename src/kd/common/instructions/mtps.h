#ifndef _MTPS_H_
#define _MTPS_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// MTPS - Move byte to Processor Status Word
//
// Operation:
//  PSW <- (src)
//
// Condition Codes:
//  Set according to effective SRC operand bits 0-3
//
// The 8 bits of the effective operand replaces the current contents of the
// PSW. The source operand address is treated as a byte address. Note that the
// T bit (PSW bit 4) cannot be set with this instruction. The SRC operand
// remains unchanged. This instruction can be used to change the priority bit
// (PSW bit 7) in the PSW
//
class CommonInstruction::MTPS : public SingleOperandInstruction, public WithFactory<MTPS>
{
public:
    MTPS (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::MTPS::MTPS (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::MTPS::execute ()
{
    CondData<u8> newValue;
    if (!readOperand (&newValue))
        return false;

    // Allow bits 5/6/7 to be set and cleared
    cpuData_->setPSW (newValue);

    return true;
}

#endif // _MTPS_H_