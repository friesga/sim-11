#ifndef _ASRB_H_
#define _ASRB_H_

#include "kdf11_ainstruction.h"
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
class KDF11_AInstruction::ASRB : public SingleOperandInstruction, public WithFactory<ASRB>
{
public:
    ASRB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline KDF11_AInstruction::ASRB::ASRB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap KDF11_AInstruction::ASRB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return CpuData::Trap::BusError;

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
        return CpuData::Trap::BusError;

    return CpuData::Trap::None;
}

#endif // _ASRB_H_