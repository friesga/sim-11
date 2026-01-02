#ifndef _ROLB_H_
#define _ROLB_H_


#include "proc/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "proc/kd/include/cpudata.h"
#include "proc/common/operandlocation/operandlocation.h"


// ROLB - rotate left byte
//
// Operation:
//  refer to ROL
// 
// Condition Codes:
//  refer to ROL
//
// Same as ROL instruction with the distinction that for odd adresses the
// carry bit is loaded with bit 15 of the word and for even addresses the carry
// bit is loaded with bit 7 of the word.
//
class ROLB : public SingleOperandInstruction
{
public:
    ROLB (u16 instruction);
};

inline ROLB::ROLB (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _ROLB_H_