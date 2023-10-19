#ifndef _BICB_H_
#define _BICB_H_

#include "commoninstruction.h"
#include "kd/common/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// BICB - bit clear byte
//
// Operation:
//  refer to BIC
// 
// Condition Codes:
//  refer to BIC
//
class CommonInstruction::BICB : public DoubleOperandInstruction, public WithFactory<BICB>
{
public:
    BICB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::BICB::BICB (CpuData* cpu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::BICB::execute ()
{
    CondData<u8> source, destination;

    if (!readSourceOperand (&source) || !readDestinationOperand (&destination))
        return CpuData::Trap::BusError;

    u8 tmp = (u8)(~source & destination);

    if (!writeDestinationOperand (tmp))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
    clearConditionCode (PSW_V);

    return CpuData::Trap::None;
}

#endif // _BICB_H_