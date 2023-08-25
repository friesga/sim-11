#ifndef _BITB_H_
#define _BITB_H_

#include "kd11_na.h"
#include "../doubleoperandinstruction/doubleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// BITB - bit test byte
//
// Operation:
//  refer to BIT
// 
// Condition Codes:
//  refer to BIT
//
class KD11_NA::BITB : public DoubleOperandInstruction, public WithFactory<BITB>
{
public:
    BITB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::BITB::BITB (CpuData* cpu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::BITB::execute ()
{
    CondData<u8> source, destination;

    if (!readSourceOperand (&source) || !readDestinationOperand (&destination))
        return CpuData::Trap::BusError;

    u16 tmp = source & destination;
    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
    clearConditionCode (PSW_V);

    return CpuData::Trap::None;
}

#endif // _BITB_H_