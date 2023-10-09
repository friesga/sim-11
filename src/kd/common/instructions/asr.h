#ifndef _ASR_H_
#define _ASR_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

// ASR - arithmetic shift right
// 
// Operation:
//  (dst) <- (dst) shifted one place to the right
//
// Condition Codes:
//  N: set if the high-order bit of the result is set (result < 0);
//     cleared otherwise
//  Z: set if the result - O; cleared otherwise
//  V: loaded from the Exclusive OR of the N-bit and C-.bit (as set by the
//     completion of the shift operation)
//  C: loaded from low-order bit of the destination
//
// Shifts all bits of the destination right one place. Bit 15 is reproduced.
// The C-bit is loaded from bit 0 of the destination. ASR performs signed
// division of the destination by two.
//
class CommonInstruction::ASR : public SingleOperandInstruction, public WithFactory<ASR>
{
public:
    ASR (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::ASR::ASR (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::ASR::execute ()
{
    CondData<u16> contents;
    if (!readOperand (&contents))
        return CpuData::Trap::BusError;

    u16 result = contents;
    if (result & 0100000)
    {
        result >>= 1;
        result |= 0100000;
    }
    else
        result >>= 1;

    if (!writeOperand (result))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_C, contents & 1);
    setConditionCodeIf_ClearElse (PSW_N, result & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    setConditionCodeIf_ClearElse (PSW_V, isSet (PSW_N) ^ isSet (PSW_C));

    return CpuData::Trap::None;
}

#endif // _ASR_H_