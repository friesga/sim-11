#ifndef _CLRB_H_
#define _CLRB_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// CLRB - clear destination byte
//
// Operation:
//  refer to CLR
// 
// Condition Codes:
//  refer to CLR
//
class CLRB : public SingleOperandInstruction
{
public:
    CLRB (u16 instruction);
};

inline CLRB::CLRB (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _CLRB_H_