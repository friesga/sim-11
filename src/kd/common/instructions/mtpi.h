#ifndef _MTPI_H_
#define _MTPI_H_

#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"

// MTPI - Move to previous instruction space
//
// Operation:
//  (tmp) <- (SP)+
//  (dst) <- (tmp)
//
// Condition Codes: 
//  N: set if the source < 0 
//  Z: set if the source = 0 
//  V: cleared 
//  C: unaffected 
//
// This instruction pops a word off the current stack determined by PS bits
// 15 and 14 and stores that word into an address in the previous space (PS
// bits 13 and 12). The destination address is computed using the current
// registers and memory map.  
//
class MTPI : public SingleOperandInstruction
{
public:
    MTPI (u16 instruction);
};

inline MTPI::MTPI (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _MTPI_H_