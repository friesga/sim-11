#ifndef _CLRB_H_
#define _CLRB_H_

#include "kd11_na.h"
#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// CLRB - clear destination byte
//
// Operation:
//  refer to CLR
// 
// Condition Codes:
//  refer to CLR
//
class KD11_NA::CLRB : public SingleOperandInstruction, public WithFactory<CLRB>
{
public:
    CLRB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::CLRB::CLRB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::CLRB::execute ()
{
    if (!writeOperand ((u8)0))
        return CpuData::Trap::BusError;

    clearConditionCode (PSW_N | PSW_V | PSW_C);
    setConditionCode (PSW_Z);

    return CpuData::Trap::None;
}

#endif // _CLRB_H_