#ifndef _FSUB_H_
#define _FSUB_H_


#include "proc/common/instructionformats/fisinstruction/fisinstruction.h"
#include "proc/include/cpudata.h"
#include "proc/common/operandlocation/operandlocation.h"


// FSUB - floating subtract
//
// Operation:
//  [(R)+4, (R)+6] <- [(R)+4, (R)+6] - [(R), (R)+2],
//  if result >= 2^-128; else [(R)+4, (R)+6] <- 0
//
// Condition Codes:
//  N: set if result < O; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: cleared
//  C: cleared
//
// Sutracts the B Argument from the A Argument and stores the result in
// the A Argument position on the stack.
//
// A <- A - B
//
class FSUB : public FISInstruction
{
public:
    FSUB (u16 instruction);
};

inline FSUB::FSUB (u16 instruction)
    :
    FISInstruction (instruction)
{}

#endif // _FSUB_H_