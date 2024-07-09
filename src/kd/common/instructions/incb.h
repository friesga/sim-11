#ifndef _INCB_H_
#define _INCB_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// INCB - increment destination byte
//
// Operation:
//  refer to INC
// 
// Condition Codes:
//  refer to INC
//
class INCB : public SingleOperandInstruction
{
public:
    INCB (u16 instruction);
};

inline INCB::INCB (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _INCB_H_