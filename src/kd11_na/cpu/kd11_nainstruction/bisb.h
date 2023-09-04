#ifndef _BISB_H_
#define _BISB_H_

#include "kd11_nainstruction.h"
#include "../doubleoperandinstruction/doubleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// BISB - bit set byte
//
// Operation:
//  refer to BIS
// 
// Condition Codes:
//  refer to BIS
//
class KD11_NAInstruction::BISB : public DoubleOperandInstruction, public WithFactory<BISB>
{
public:
    BISB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::BISB::BISB (CpuData* cpu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::BISB::execute ()
{
    CondData<u8> source, destination;

    if (!readSourceOperand (&source) || !readDestinationOperand (&destination))
        return CpuData::Trap::BusError;

    u8 tmp = source | destination;

    if (!writeDestinationOperand (tmp))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
    clearConditionCode (PSW_V);

    return CpuData::Trap::None;
}

#endif // _BISB_H_