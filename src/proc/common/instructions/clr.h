#ifndef _CLR_H_
#define _CLR_H_


#include "proc/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "proc/include/cpudata.h"
#include "proc/common/operandlocation/operandlocation.h"


// CLR - clear destination
//
// Operation:
//  (dst) <- 0
//
// Condition Codes:
//  N: cleared
//  Z: set
//  V: cleared
//  C: cleared
//
// Contents of specified destination are replaced with zeroes.
//
class CLR : public SingleOperandInstruction
{
public:
    CLR (u16 instruction);
};

inline CLR::CLR (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _CLR_H_