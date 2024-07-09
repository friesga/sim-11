#ifndef _FDIV_H_
#define _FDIV_H_


#include "kd/common/instructionformats/fisinstruction/fisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// FDIV - floating divide
//
// Operation:
//  [(R)+4, (R)+6] <- [(R)+4, (R)+6] / [(R),(R)+2 ]
//  if result >= 2^-128; else [(R)+4, (R)+6)] <- 0
//
// Condition Codes:
//  N: set if result < O; cleared otherwise
//  Z: set if result = 0; cleared otherwise
//  V: cleared
//  C: cleared
//
// Divides the A Argument by the B Argument and stores the result in the
// A Argument position on the stack. If the divisor (B Argument) is equal to
// zero, the stack is left untouched.
//
// A <- A / B
//
class FDIV : public FISInstruction
{
public:
    FDIV (u16 instruction);
};

inline FDIV::FDIV (u16 instruction)
    :
    FISInstruction (instruction)
{}

#endif // _FDIV_H_