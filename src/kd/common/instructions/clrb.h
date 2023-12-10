#ifndef _CLRB_H_
#define _CLRB_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// CLRB - clear destination byte
//
// Operation:
//  refer to CLR
// 
// Condition Codes:
//  refer to CLR
//
class CommonInstruction::CLRB : public SingleOperandInstruction, public WithFactory<CLRB>
{
public:
    CLRB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::CLRB::CLRB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::CLRB::execute ()
{
    if (!writeOperand ((u8)0))
        return false;

    setPSW (ConditionCodes {.N = false,
        .Z = true,
        .V = false,
        .C = false});

    return true;
}

#endif // _CLRB_H_