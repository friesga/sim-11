#ifndef _NEG_H_
#define _NEG_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// NEG - negate destination
//
// Operation:
//  (dst) <- -(dst)
//
// Condition Codes:
//  N: set if the result is < 0; cleared otherwise
//  Z: set if result is O: cleared otherwise
//  V: set if the result is 100000; cleared otherwise
//  C: cleared if the result is O; set otherwise
//
// Replaces the contents of the destination address by its two's complement.
// Note that 100000 is replaced by itself (in two's complement notation the
// most negative number has no positive counterpart).
//
class NEG : public SingleOperandInstruction
{
public:
    NEG (u16 instruction);
};

inline NEG::NEG (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _NEG_H_