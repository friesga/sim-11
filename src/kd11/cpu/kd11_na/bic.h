#ifndef _BIC_H_
#define _BIC_H_

#include "kd11_na.h"
#include "../doubleoperandinstruction/doubleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// BIC - bit clear
//
// Operation:
//  (dst) <- (src) ^ (dst)
//
// Condition Codes:
//  N: set if high order bit of result set; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: cleared
//  C: not affected
//
// Clears each bit in the destination that corresponds to a set bit in the
// source. The original contents of the destination are lost. The contents of
// the source are unaffected.
//
class KD11_NAInstruction::BIC : public DoubleOperandInstruction, public WithFactory<BIC>
{
public:
    BIC (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::BIC::BIC (CpuData* cpu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::BIC::execute ()
{
    CondData<u16> source, destination;

    if (!readSourceOperand (&source) ||
        !readDestinationOperand (&destination))
        return CpuData::Trap::BusError;

    u16 result = ~source & destination;

    if (!writeDestinationOperand (result))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_N, result & 0x8000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    clearConditionCode (PSW_V);

    return CpuData::Trap::None;
}

#endif // _BIC_H_