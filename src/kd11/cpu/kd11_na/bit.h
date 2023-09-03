#ifndef _BIT_H_
#define _BIT_H_

#include "kd11_na.h"
#include "../doubleoperandinstruction/doubleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// BIT - bit test
//
// Operation:
//  (src) ^ (dst)
//
// Condition Codes:
//  N: set if high-order bit of result set: cleared otherwise
//  Z: set if result = O: cleared otherwise
//  V: cleared
//  C: not affected
//
// Performs logical "and"comparison of the source and destination operands
// and modifies condition codes accordingly. Neither the source nor
// destination operands are affected.
//
class KD11_NAInstruction::BIT : public DoubleOperandInstruction, public WithFactory<BIT>
{
public:
    BIT (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::BIT::BIT (CpuData* cpu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::BIT::execute ()
{
    CondData<u16> source, destination;

    if (!readSourceOperand (&source) ||
        !readDestinationOperand (&destination))
        return CpuData::Trap::BusError;

    u16 tmp = source & destination;
    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x8000);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
    clearConditionCode (PSW_V);

    return CpuData::Trap::None;
}

#endif // _BIT_H_