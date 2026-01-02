#ifndef _BITB_H_
#define _BITB_H_


#include "proc/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "proc/include/cpudata.h"
#include "proc/common/operandlocation/operandlocation.h"


// BITB - bit test byte
//
// Operation:
//  refer to BIT
// 
// Condition Codes:
//  refer to BIT
//
class BITB : public DoubleOperandInstruction
{
public:
    BITB (u16 instruction);
};

inline BITB::BITB (u16 instruction)
    :
    DoubleOperandInstruction (instruction)
{}

#endif // _BITB_H_