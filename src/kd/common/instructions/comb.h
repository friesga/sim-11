#ifndef _COMB_H_
#define _COMB_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// COMB - complement destination byte
//
// Operation:
//  refer to COM
// 
// Condition Codes:
//  refer to COM
//
class CommonInstruction::COMB : public SingleOperandInstruction, public WithFactory<COMB>
{
public:
    COMB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::COMB::COMB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::COMB::execute ()
{
    CondData<u8> operand;
    if (!readOperand (&operand))
        return false;

    // Complement the operand and write it to the operand location
    operand = ~operand;
    if (!writeOperand (operand.value ()))
        return false;

    setPSW ({ConditionCodes {.N = (bool) (operand & 0x80),
        .Z = (u8) operand == 0,
        .V = false,
        .C = true}});

    return true;
}

#endif // _COMB_H_