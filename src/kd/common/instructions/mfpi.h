#ifndef _MFPI_H_
#define _MFPI_H_

#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"

// MFPI - Move from previous instruction space
//
// Operation:
//  (tmp) <- (src)
//  -(SP) <- (tmp)
//
// Condition Codes: 
//  N: set if the source < 0 
//  Z: set if the source = 0 
//  V: cleared 
//  C: unaffected 
//
// Pushes a word onto the current stack from an address in the previous space.
// The source address is computed using the current registers and memory map.
//
class MFPI : public SingleOperandInstruction
{
public:
    MFPI (u16 instruction);
};

inline MFPI::MFPI (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _MFPI_H_