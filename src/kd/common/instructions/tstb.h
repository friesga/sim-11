#ifndef _TSTB_H_
#define _TSTB_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// TSTB - test destination byte
//
// Operation:
//  refer to TST
// 
// Condition Codes:
//  refer to TST
//
class TSTB : public SingleOperandInstruction
{
public:
    TSTB (u16 instruction);
};

inline TSTB::TSTB (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _TSTB_H_