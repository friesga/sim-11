#ifndef _ROR_H_
#define _ROR_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

// ROR - rotate right
//
// Operation:
//  (dst) <- (dst) rotated right one place
//
// Condition Codes:
//  N: set if the high-order bit of the result is set (result < 0);
//     cleared otherwise
//  Z: set if all bits of result - 0; cleared otherwise
//  V: loaded with the Exclusive OR of the N-bit and C-bi.t (as set by the
//     completion of the rotate operation)
//  C: loaded with the low-order bit of the destination
//
// Rotates all bits of the destination right one place. Bit 0 is loaded into
// the C-bit and the previous contents of the C-bit are loaded into bit 15 of
// the destination.
//
class CommonInstruction::ROR : public SingleOperandInstruction, public WithFactory<ROR>
{
public:
    ROR (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::ROR::ROR (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::ROR::execute ()
{
    CondData<u16> contents;
    if (!readOperand (&contents))
        return CpuData::Trap::BusError;

    u16 result = contents >> 1;
    if (isSet (PSW_C))
        result |= 0100000;

    if (!writeOperand (result))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_C, contents & 0000001);
    setConditionCodeIf_ClearElse (PSW_N, result & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    setConditionCodeIf_ClearElse (PSW_V, isSet (PSW_N) ^ isSet (PSW_C));

    return CpuData::Trap::None;
}

#endif // _ROR_H_