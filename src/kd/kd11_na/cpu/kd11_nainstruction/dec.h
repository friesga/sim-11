#ifndef _DEC_H_
#define _DEC_H_

#include "kd11_nainstruction.h"
#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// DEC - decrement destination
//
// Operation:
//  (dst) <- (dst) - 1
//
// Condition Codes:
//  N: set if result is <0; cleared otherwise
//  Z: set if result is 0; cleared otherwise
//  V: set if (dst) was 100000; cleared otherwise
//  C: not affected
//
// Subtract 1 from the contents of the destination
//
class KD11_NAInstruction::DEC : public SingleOperandInstruction, public WithFactory<DEC>
{
public:
    DEC (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::DEC::DEC (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::DEC::execute ()
{
    CondData<u16> contents;
    if (!readOperand (&contents))
        return CpuData::Trap::BusError;

    // Increment the operand and write it to the operand location
    u16 result = contents - 1;
    if (!writeOperand (result))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_V, contents == 0100000);
    setConditionCodeIf_ClearElse (PSW_N, result & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);

    return CpuData::Trap::None;
}

#endif // _DEC_H_