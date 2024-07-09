#ifndef _NEGB_H_
#define _NEGB_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// NEGB - negate destination byte
//
// Operation:
//  refer to NEG
// 
// Condition Codes:
//  refer to NEG
//
class NEGB : public SingleOperandInstruction
{
public:
    NEGB (u16 instruction);
};

inline NEGB::NEGB (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _NEGB_H_