#ifndef _CLRB_H_
#define _CLRB_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

// CLRB - clear destination byte
//
// Operation:
//  refer to CLR
// 
// Condition Codes:
//  refer to CLR
//
class CommonInstruction::CLRB : public SingleOperandInstruction, public WithFactory<CLRB>
{
public:
    CLRB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::CLRB::CLRB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::CLRB::execute ()
{
    if (!writeOperand ((u8)0))
        return CpuData::Trap::BusError;

    clearConditionCode (PSW_N | PSW_V | PSW_C);
    setConditionCode (PSW_Z);

    return CpuData::Trap::None;
}

#endif // _CLRB_H_