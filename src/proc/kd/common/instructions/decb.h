#ifndef _DECB_H_
#define _DECB_H_


#include "proc/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "proc/kd/include/cpudata.h"
#include "proc/kd/common/operandlocation/operandlocation.h"


// DECB - decrement destination byte
//
// Operation:
//  refer to DEC
// 
// Condition Codes:
//  refer to DEC
//
class DECB : public SingleOperandInstruction
{
public:
    DECB (u16 instruction);
};

inline DECB::DECB (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _DECB_H_