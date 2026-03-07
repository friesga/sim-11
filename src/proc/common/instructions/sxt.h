#ifndef _SXT_H_
#define _SXT_H_


#include "proc/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "proc/include/cpudata.h"
#include "proc/common/operandlocation/operandlocation.h"


// SXT - sign extend
//
// Operation:
//  (dst) <- 0 if N bit is clear
//  (dst) <- 1 if N bit is set
//
// Condition Codes:
//  N: unaffected
//  Z: set if N bit clear
//  V: cleared
//  C: unaffected
//
// If the condition code bit N is set then a -1 is placed in the destination
// operand: if N bit is clear, then a 0 is placed in the destination operand.
//
class SXT : public SingleOperandInstruction
{
public:
    SXT (u16 instruction);
};

inline SXT::SXT (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _SXT_H_