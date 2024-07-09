#ifndef _BIC_H_
#define _BIC_H_


#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// BIC - bit clear
//
// Operation:
//  (dst) <- (src) ^ (dst)
//
// Condition Codes:
//  N: set if high order bit of result set; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: cleared
//  C: not affected
//
// Clears each bit in the destination that corresponds to a set bit in the
// source. The original contents of the destination are lost. The contents of
// the source are unaffected.
//
class BIC : public DoubleOperandInstruction
{
public:
    BIC (u16 instruction);
};

inline BIC::BIC (u16 instruction)
    :
    DoubleOperandInstruction (instruction)
{}

#endif // _BIC_H_