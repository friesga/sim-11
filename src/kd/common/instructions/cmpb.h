#ifndef _CMPB_H_
#define _CMPB_H_


#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// CMPB - compare source to destination byte
// 
// Operation:
//  refer to CMP
// 
// Condition Codes:
//  refer to CMP
//
class CMPB : public DoubleOperandInstruction
{
public:
    CMPB (u16 instruction);
};

inline CMPB::CMPB (u16 instruction)
    :
    DoubleOperandInstruction (instruction)
{}

#endif // _CMPB_H_