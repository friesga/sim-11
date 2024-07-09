#ifndef _MOVB_H_
#define _MOVB_H_


#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// MOVB - move source to destination
// 
// Operation:
//  refer to MOV
// 
// Condition Codes:
//  refer to MOV
//
// The MOVB to a register (unique among byte instructions) extends the most
// significant bit of the low order byte (sign extension). Otherwise MOVB
// operates on bytes exactly as MOV operates on words.
//
class MOVB : public DoubleOperandInstruction
{
public:
    MOVB (u16 instruction);
};

inline MOVB::MOVB (u16 instruction)
    :
    DoubleOperandInstruction (instruction)
{}


#endif // _MOVB_H_