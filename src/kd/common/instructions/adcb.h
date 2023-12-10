#ifndef _ADCB_H_
#define _ADCB_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// ADCB - add carry byte
//
// Operation:
//  refer to ADC
// 
// Condition Codes:
//  refer to ADC
//
class CommonInstruction::ADCB : public SingleOperandInstruction, public WithFactory<ADCB>
{
public:
    ADCB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::ADCB::ADCB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::ADCB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return false;

    u16 tmp = isSet (PSW_C) ? 1 : 0;
    u8 destination = (u8)(source + tmp);

    if (!writeOperand (destination))
        return false;

    setPSW (ConditionCodes ({.N = (bool) (destination & 0x80),
        .Z = destination == 0,
        .V = source == 0177 && isSet (PSW_C),
        .C = source == 0377 && isSet (PSW_C)}));

    return true;
}

#endif // _ADCB_H_