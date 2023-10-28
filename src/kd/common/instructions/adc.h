#ifndef _ADC_H_
#define _ADC_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

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
class CommonInstruction::ADC : public SingleOperandInstruction, public WithFactory<ADC>
{
public:
    ADC (CpuData* cpu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::ADC::ADC (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline bool CommonInstruction::ADC::execute ()
{
    CondData<u16> contents;
    if (!readOperand (&contents))
        return false;

    u16 cBit = isSet (PSW_C) ? 1 : 0;
    u16 result = contents + cBit;

    if (!writeOperand (result))
        return false;

     setPSW (ConditionCodes ({.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = contents == 0077777 && isSet (PSW_C),
        .C = contents == 0177777 && isSet (PSW_C)}));

    return true;
}

#endif // _ADC_H_