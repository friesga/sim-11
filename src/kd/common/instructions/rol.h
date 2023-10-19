#ifndef _ROL_H_
#define _ROL_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// ROL - rotate left
// 
// Operation:
//  (dst) <- (dst) rotated left one place
//
// Condition Codes:
//  N: set if the high-order bit of the result word is set (result < 0);
//     cleared otherwise
//  Z: set if all bits of the result word = O; cleared otherwise
//  V: loaded with the Exclusive OR of the N-bit and C-bit (as set by the
//     completion of the rotate operation)
//  C: loaded with the high-order bit of the destination
//
// Rotate all bits of the destination left one place. Bit 15 is loaded into
// the C-bit of the status word and the previous contents of the C-bit are
// loaded into Bit 0 of the destination.
//
class CommonInstruction::ROL : public SingleOperandInstruction, public WithFactory<ROL>
{
public:
    ROL (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::ROL::ROL (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::ROL::execute ()
{
    CondData<u16> contents;
    if (!readOperand (&contents))
        return CpuData::Trap::BusError;

    u16 cBit = isSet (PSW_C);
    u16 result = contents << 1;
    if (cBit)
        result |= 01;

    if (!writeOperand (result))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_C, contents & 0100000);
    setConditionCodeIf_ClearElse (PSW_N, result & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    setConditionCodeIf_ClearElse (PSW_V, isSet (PSW_N) ^ isSet (PSW_C));

    return CpuData::Trap::None;
}

#endif // _ROL_H_