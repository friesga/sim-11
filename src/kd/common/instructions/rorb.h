#ifndef _RORB_H_
#define _RORB_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// RORB - rotate right byte
//
// Operation:
//  refer to ROR
// 
// Condition Codes:
//  refer to ROR
//
// Same as ROR instruction with the distintion that for odd adresses the
// carry bit is loaded in bit 15 of the word and for even addresses the carry
// bit is loaded in bit 7 of the word.
//
class RORB : public SingleOperandInstruction
{
public:
    RORB (u16 instruction);
};

inline RORB::RORB (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _RORB_H_