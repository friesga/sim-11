#ifndef _DECB_H_
#define _DECB_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// DECB - decrement destination byte
//
// Operation:
//  refer to DEC
// 
// Condition Codes:
//  refer to DEC
//
class CommonInstruction::DECB : public SingleOperandInstruction, public WithFactory<DECB>
{
public:
    DECB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::DECB::DECB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::DECB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return false;

    u8 result = (u8) (source - 1);

    if (!writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = source == 0000200});

    return true;
}

#endif // _DECB_H_