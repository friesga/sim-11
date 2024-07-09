#ifndef _ADC_H_
#define _ADC_H_

#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


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
class ADC : public SingleOperandInstruction
{
public:
    ADC (u16 instruction);
};

inline ADC::ADC (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _ADC_H_