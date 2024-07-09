#ifndef _INC_H_
#define _INC_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// INC - increment destination
//
// Operation:
//  (dst) <- (dst) + 1
//
// Condition Codes:
//  N: set if result is <0; cleared otherwise
//  Z: set if result is O; cleared otherwise
//  V: set if (dst) held 077777; cleared otherwise
//  C: not affected
//
// Add one to contents of destination
//
class INC : public SingleOperandInstruction
{
public:
    INC (u16 instruction);
};

inline INC::INC (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _INC_H_