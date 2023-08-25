#ifndef _SUB_H_
#define _SUB_H_

#include "kd11_na.h"
#include "../doubleoperandinstruction/doubleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// SUB - subtract src from dst
//
// Operation:
//  (dst) <- (dst) - (src)
//
// Condition Codes:
//  N: set if result <0; cleared otherwise
//  Z: set if result = 0; cleared otherwise
//  V: set if there was arithmetic overflow as a result of the operation,
//     that is if operands were of opposite signs and the sign of the source
//     was the same as the sign of the result; cleared otherwise
//  C: cleared if there was a carry from the most significant bit of the
//     result; set otherwise
//
class KD11_NA::SUB : public DoubleOperandInstruction, public WithFactory<SUB>
{
public:
    SUB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::SUB::SUB (CpuData* cpu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::SUB::execute ()
{
    CondData<u16> source, destination;

    if (!readSourceOperand (&source) ||
        !readDestinationOperand (&destination))
        return CpuData::Trap::BusError;

    u16 result = destination - source;

    if (!writeDestinationOperand (result))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_N, result & 0x8000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    setConditionCodeIf_ClearElse (PSW_V, ((source & 0x8000) != (destination & 0x8000)) \
        && ((source & 0x8000) == (result & 0x8000)));
    setConditionCodeIf_ClearElse (PSW_C, ((u32)destination - (u32)source) & 0x10000);

    return CpuData::Trap::None;
}

#endif // _SUB_H_