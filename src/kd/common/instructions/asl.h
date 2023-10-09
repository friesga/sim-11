#ifndef _ASL_H_
#define _ASL_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

// ASL - arithmetic shift left
//
// Operation:
//  (dst) <- (dst) shifted one place to the left
//
// Condition Codes:
//  N: set if high-order bit of the result is set (result < 0); cleared
//     otherwise
//  Z: set if the result = O; cleared otherwise
//  V: loaded with the exclusive OR of the N-bit and C-bit (as set by the
//     completion of the shift operation)
//  C: loaded with the high-order bit of the destination
//
// Shifts all bits of the destination left one place. Bit 0 is loaded with
// an 0. The C-bit of the status word is loaded from the most significant bit
// of the destination. ASL performs a signed multiplication of the destination
// by 2 with overflow indication.
//
class CommonInstruction::ASL : public SingleOperandInstruction, public WithFactory<ASL>
{
public:
    ASL (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::ASL::ASL (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::ASL::execute ()
{
    CondData<u16> contents;
    if (!readOperand (&contents))
        return CpuData::Trap::BusError;

    u16 result = contents << 1;

    if (!writeOperand (result))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_C, contents & 0100000);
    setConditionCodeIf_ClearElse (PSW_N, result & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    setConditionCodeIf_ClearElse (PSW_V, isSet (PSW_N) ^ isSet (PSW_C));

    return CpuData::Trap::None;
}

#endif // _ASL_H_