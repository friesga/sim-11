#ifndef _BITB_H_
#define _BITB_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

// BITB - bit test byte
//
// Operation:
//  refer to BIT
// 
// Condition Codes:
//  refer to BIT
//
class CommonInstruction::BITB : public DoubleOperandInstruction, public WithFactory<BITB>
{
public:
    BITB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::BITB::BITB (CpuData* cpu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::BITB::execute ()
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