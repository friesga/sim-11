#ifndef _INCB_H_
#define _INCB_H_

#include "kdf11instruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// INCB - increment destination byte
//
// Operation:
//  refer to INC
// 
// Condition Codes:
//  refer to INC
//
class KDF11Instruction::INCB : public SingleOperandInstruction, public WithFactory<INCB>
{
public:
    INCB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11Instruction::INCB::INCB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11Instruction::INCB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return false;

    u8 result = (u8)(source + 1);

    setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = source == 000177});

    if (!writeOperand (result))
        return false;

    return true;
}

#endif // _INCB_H_