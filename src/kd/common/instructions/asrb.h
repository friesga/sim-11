#ifndef _ASRB_H_
#define _ASRB_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// ASRB - arithmetic shift right byte
//
// Operation:
//  refer to ASR
// 
// Condition Codes:
//  refer to ASR
//
// Same as ASR instruction with the distinction that for odd adresses bit 15
// is reproduced and for even addresses bit 7 is reproduced.
//
class ASRB : public SingleOperandInstruction
{
public:
    ASRB (u16 instruction);
};

inline ASRB::ASRB (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _ASRB_H_