#ifndef _NEG_H_
#define _NEG_H_

#include "kd11_na.h"
#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// NEG - negate destination
//
// Operation:
//  (dst) <- -(dst)
//
// Condition Codes:
//  N: set if the result is < 0; cleared otherwise
//  Z: set if result is O: cleared otherwise
//  V: set if the result is 100000; cleared otherwise
//  C: cleared if the result is O; set otherwise
//
// Replaces the contents of the destination address by its two's complement.
// Note that 100000 is replaced by itself (in two's complement notation the
// most negative number has no positive counterpart).
//
class KD11_NA::NEG : public SingleOperandInstruction, public WithFactory<NEG>
{
public:
    NEG (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::NEG::NEG (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::NEG::execute ()
{
    CondData<u16> operand;
    if (!readOperand (&operand))
        return CpuData::Trap::BusError;

    // Negate the operand and write it to the operand location
    if (operand != 0100000)
        operand = -operand;

    if (!writeOperand (operand.value ()))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_V, operand == 0100000);
    setConditionCodeIf_ClearElse (PSW_N, operand & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !operand);
    setConditionCodeIf_ClearElse (PSW_C, operand);

    return CpuData::Trap::None;
}

#endif // _NEG_H_