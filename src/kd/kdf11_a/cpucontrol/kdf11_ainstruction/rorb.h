#ifndef _RORB_H_
#define _RORB_H_

#include "kdf11_ainstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// RORB - rotate right byte
//
// Operation:
//  refer to ROR
// 
// Condition Codes:
//  refer to ROR
//
// Same as ROR instruction with the distintion that for odd adresses the
// carry bit is loaded in bit 15 of the word and for even addresses the carry
// bit is loaded in bit 7 of the word.
//
class KDF11_AInstruction::RORB : public SingleOperandInstruction, public WithFactory<RORB>
{
public:
    RORB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::RORB::RORB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11_AInstruction::RORB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return false;

    u8 result = source >> 1;
    if (isSet (PSW_C))
        result |= 0x80;

    setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = (bool) (result & 0x80) != (bool) (source & 0x01),
        .C = (bool) (source & 0x01)});

    if (!writeOperand (result))
        return false;

    return true;
}

#endif // _RORB_H_