#ifndef _COMB_H_
#define _COMB_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// COMB - complement destination byte
//
// Operation:
//  refer to COM
// 
// Condition Codes:
//  refer to COM
//
class COMB : public SingleOperandInstruction
{
public:
    COMB (u16 instruction);
};

inline COMB::COMB (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _COMB_H_