#ifndef _TSTB_H_
#define _TSTB_H_

#include "kd11_na.h"
#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// TSTB - test destination byte
//
// Operation:
//  refer to TST
// 
// Condition Codes:
//  refer to TST
//
class KD11_NA::TSTB : public SingleOperandInstruction, public WithFactory<TSTB>
{
public:
    TSTB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::TSTB::TSTB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::TSTB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return CpuData::Trap::BusError;

    clearConditionCode (PSW_V);
    clearConditionCode (PSW_C);
    setConditionCodeIf_ClearElse (PSW_N, source & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !source);

    return CpuData::Trap::None;
}

#endif // _TSTB_H_