#ifndef _ROR_H_
#define _ROR_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// ROR - rotate right
//
// Operation:
//  (dst) <- (dst) rotated right one place
//
// Condition Codes:
//  N: set if the high-order bit of the result is set (result < 0);
//     cleared otherwise
//  Z: set if all bits of result - 0; cleared otherwise
//  V: loaded with the Exclusive OR of the N-bit and C-bi.t (as set by the
//     completion of the rotate operation)
//  C: loaded with the low-order bit of the destination
//
// Rotates all bits of the destination right one place. Bit 0 is loaded into
// the C-bit and the previous contents of the C-bit are loaded into bit 15 of
// the destination.
//
class ROR : public SingleOperandInstruction
{
public:
    ROR (u16 instruction);
};

inline ROR::ROR (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _ROR_H_