#ifndef _CLR_H_
#define _CLR_H_

#include "kd11_na.h"
#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// CLR - clear destination
//
// Operation:
//  (dst) <- 0
//
// Condition Codes:
//  N: cleared
//  Z: set
//  V: cleared
//  C: cleared
//
// Contents of specified destination are replaced with zeroes.
//
class KD11_NAInstruction::CLR : public SingleOperandInstruction, public WithFactory<CLR>
{
public:
    CLR (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::CLR::CLR (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::CLR::execute ()
{
    if (!writeOperand ((u16)0))
        return CpuData::Trap::BusError;

    clearConditionCode (PSW_N | PSW_V | PSW_C);
    setConditionCode (PSW_Z);

    return CpuData::Trap::None;
}

#endif // _CLR_H_