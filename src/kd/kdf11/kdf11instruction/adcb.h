#ifndef _ADCB_H_
#define _ADCB_H_

#include "kdf11instruction.h"
#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// ADCB - add carry byte
//
// Operation:
//  refer to ADC
// 
// Condition Codes:
//  refer to ADC
//
class KDF11Instruction::ADCB : public SingleOperandInstruction, public WithFactory<ADCB>
{
public:
    ADCB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11Instruction::ADCB::ADCB (CpuData* cpuData,
        CpuControl* cpuControl, MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11Instruction::ADCB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return false;

    u16 tmp = isSet (PSW_C) ? 1 : 0;
    u8 destination = (u8) (source + tmp);

    setPSW (ConditionCodes ({.N = (bool) (destination & 0x80),
        .Z = destination == 0,
        .V = source == 0177 && isSet (PSW_C),
        .C = source == 0377 && isSet (PSW_C)}));

    if (!writeOperand (destination))
        return false;

    return true;
}

#endif // _ADCB_H_