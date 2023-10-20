#ifndef _RORB_H_
#define _RORB_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

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
class CommonInstruction::RORB : public SingleOperandInstruction, public WithFactory<RORB>
{
public:
    RORB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::RORB::RORB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::RORB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return CpuData::Trap::BusError;

    u8 result = source >> 1;
    if (isSet (PSW_C))
        result |= 0x80;

    if (!writeOperand (result))
        return CpuData::Trap::BusError;

    setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = (result & 0x80) != (source & 0x01),
        .C = (bool) (source & 0x01)});

    return CpuData::Trap::None;
}

#endif // _RORB_H_