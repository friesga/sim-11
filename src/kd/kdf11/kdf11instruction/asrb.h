#ifndef _ASRB_H_
#define _ASRB_H_

#include "kdf11instruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// ASRB - arithmetic shift right byte
//
// Operation:
//  refer to ASR
// 
// Condition Codes:
//  refer to ASR
//
// Same as ASR instruction with the distinction that for odd adresses bit 15
// is reproduced and for even addresses bit 7 is reproduced.
//
class KDF11Instruction::ASRB : public SingleOperandInstruction, public WithFactory<ASRB>
{
public:
    ASRB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11Instruction::ASRB::ASRB (CpuData* cpuData,
        CpuControl* cpuControl, MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11Instruction::ASRB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return false;

    u8 result = source;
    if (result & 0x80)
    {
        result >>= 1;
        result |= 0x80;
    }
    else
        result >>= 1;

    setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = (bool) (result & 0x80) != (bool) (source & 1),
        .C = (bool) (source & 1)});

    if (!writeOperand (result))
        return false;

    return true;
}

#endif // _ASRB_H_