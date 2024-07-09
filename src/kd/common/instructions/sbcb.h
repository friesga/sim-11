#ifndef _SBCB_H_
#define _SBCB_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// SBCB - subtract carry byte
//
// Operation:
//  refer to SBC
// 
// Condition Codes:
//  refer to SBC
//
class SBCB : public SingleOperandInstruction
{
public:
    SBCB (u16 instruction);
};

inline SBCB::SBCB (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _SBCB_H_