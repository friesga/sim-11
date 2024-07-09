#ifndef _BICB_H_
#define _BICB_H_


#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// BICB - bit clear byte
//
// Operation:
//  refer to BIC
// 
// Condition Codes:
//  refer to BIC
//
class BICB : public DoubleOperandInstruction
{
public:
    BICB (u16 instruction);
};

inline BICB::BICB (u16 instruction)
    :
    DoubleOperandInstruction (instruction)
{}

#endif // _BICB_H_