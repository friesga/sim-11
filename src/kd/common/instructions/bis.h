#ifndef _BIS_H_
#define _BIS_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/kd11_na/cpu/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

// BIS - bit set
//
// Operation:
//  (dst) <- (src) v (dst)
//
// Condition Codes:
//  N: set if high-order bit of result set. cleared otherwise
//  Z: set if result = O: cleared otherwise
//  V: cleared
//  C: not affected
//
// Performs "Inclusive OR"operation between the source and destination
// operands and leaves the result at the destination address: that is,
// corresponding bits set in the source are set in the destination. The
// contents of the destination are lost.
//
class CommonInstruction::BIS : public DoubleOperandInstruction, public WithFactory<BIS>
{
public:
    BIS (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::BIS::BIS (CpuData* cpu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::BIS::execute ()
{
    CondData<u16> source, destination;

    if (!readSourceOperand (&source) ||
        !readDestinationOperand (&destination))
        return CpuData::Trap::BusError;

    u16 tmp = source | destination;

    if (!writeDestinationOperand (tmp))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x8000);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
    clearConditionCode (PSW_V);

    return CpuData::Trap::None;
}

#endif // _BIS_H_