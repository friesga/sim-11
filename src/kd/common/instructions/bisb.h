#ifndef _BISB_H_
#define _BISB_H_


#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// BISB - bit set byte
//
// Operation:
//  refer to BIS
// 
// Condition Codes:
//  refer to BIS
//
class BISB : public DoubleOperandInstruction
{
public:
    BISB (u16 instruction);
};

inline BISB::BISB (u16 instruction)
    :
    DoubleOperandInstruction (instruction)
{}

#endif // _BISB_H_