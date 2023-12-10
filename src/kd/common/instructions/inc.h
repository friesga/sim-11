#ifndef _INC_H_
#define _INC_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// INC - increment destination
//
// Operation:
//  (dst) <- (dst) + 1
//
// Condition Codes:
//  N: set if result is <0; cleared otherwise
//  Z: set if result is O; cleared otherwise
//  V: set if (dst) held 077777; cleared otherwise
//  C: not affected
//
// Add one to contents of destination
//
class CommonInstruction::INC : public SingleOperandInstruction, public WithFactory<INC>
{
public:
    INC (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::INC::INC (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::INC::execute ()
{
    CondData<u16> contents;
    if (!readOperand (&contents))
        return false;

    // Increment the operand and write it to the operand location
    u16 result = contents + 1;
    if (!writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = contents == 077777});

    return true;
}

#endif // _INC_H_