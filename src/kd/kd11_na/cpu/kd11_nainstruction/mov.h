#ifndef _MOV_H_
#define _MOV_H_

#include "kd11_nainstruction.h"
#include "../doubleoperandinstruction/doubleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// MOV - move source to destination
//
// Operation:
//  (dst) <- (src)
//
// Condition Codes:
//  N: set if (src) < 0: cleared otherwise
//  Z: set if (src) = 0: cleared otherwise
//  V: cleared
//  C: not affected
//
// Moves the source operand to the destination location. The previous
// contents of the destination are lost. The contents of the source address
// are not affected.
//
class KD11_NAInstruction::MOV : public DoubleOperandInstruction, public WithFactory<MOV>
{
public:
    MOV (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::MOV::MOV (CpuData* cpu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::MOV::execute ()
{
    CondData<u16> source, destination;

    if (!readSourceOperand (&source) ||
        !writeDestinationOperand (source.value ()))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_N, source & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !source);
    clearConditionCode (PSW_V);

    return CpuData::Trap::None;
}

#endif // _MOV_H_