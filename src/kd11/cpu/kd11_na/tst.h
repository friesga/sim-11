#ifndef _TST_H_
#define _TST_H_

#include "kd11_na.h"
#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// TST - test destination
//
// Operation
// (dst) <- (dst)
//
// Condition Codes:
//  N: set if the result is < 0; cleared otherwise
//  Z: set if result is 0; cleared otherwise
//  V: cleared
//  C: cleared
//
class KD11_NAInstruction::TST : public SingleOperandInstruction, public WithFactory<TST>
{
public:
    TST (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::TST::TST (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::TST::execute ()
{
    CondData<u16> contents;
    if (!readOperand (&contents))
        return CpuData::Trap::BusError;

    clearConditionCode (PSW_V);
    clearConditionCode (PSW_C);
    setConditionCodeIf_ClearElse (PSW_N, contents & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !contents);

    return CpuData::Trap::None;
}

#endif // _TST_H_