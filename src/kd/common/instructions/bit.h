#ifndef _BIT_H_
#define _BIT_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
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
class CommonInstruction::BIT : public DoubleOperandInstruction, public WithFactory<BIT>
{
public:
    BIT (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::BIT::BIT (CpuData* cpu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::BIT::execute ()
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