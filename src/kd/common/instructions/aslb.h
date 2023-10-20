#ifndef _ASLB_H_
#define _ASLB_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// ASLB - arithmetic shift left byte
//
// Operation:
//  refer to ASL
// 
// Condition Codes:
//  refer to ASL
//
// Same as ASL instruction with the distinction that for odd adresses the
// carry bit is loaded from bit 15 of the word and bit 8 is loaded with a zero
// and for even addresses the carry bit is loaded from bit 7 and bit 0 is
// loaded with a zero.
//
class CommonInstruction::ASLB : public SingleOperandInstruction, public WithFactory<ASLB>
{
public:
    ASLB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::ASLB::ASLB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::ASLB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return CpuData::Trap::BusError;

    u8 result = (u8)(source << 1);

    if (!writeOperand (result))
        return CpuData::Trap::BusError;

    setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = (bool) ((result & 0x80) ^ (source & 0x80)),
        .C = (bool) (source & 0x80)});

    return CpuData::Trap::None;
}

#endif // _ASLB_H_