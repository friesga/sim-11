#ifndef _ADC_H_
#define _ADC_H_

#include "kdf11instruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// ADC - add carry
//
// Operation:
//  (dst) <- (dst) + (C bit)
//
// Condition Codes:
//  N: set if result <0; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: set if (dst) was 077777 and (C) was l; cleared otherwise
//  C: set if (dst) was 177777 and (C) was l; cleared otherwise
//
// Adds the contents of the C-bit into the destination. This permits the
// carry from the addition of the low-order words to be carried into the
// high-order result.
//
class KDF11_AInstruction::ADC : public SingleOperandInstruction, public WithFactory<ADC>
{
public:
    ADC (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::ADC::ADC (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11_AInstruction::ADC::execute ()
{
    CondData<u16> contents;
    if (!readOperand (&contents))
        return false;

    u16 cBit = isSet (PSW_C) ? 1 : 0;
    u16 result = contents + cBit;

    setPSW (ConditionCodes ({.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = contents == 0077777 && isSet (PSW_C),
        .C = contents == 0177777 && isSet (PSW_C)}));

    if (!writeOperand (result))
        return false;

    return true;
}

#endif // _ADC_H_