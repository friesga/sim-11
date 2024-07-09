#ifndef _MTPD_H_
#define _MTPD_H_

#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"


// MTPD - Move to previous data space
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
// Since data space does not exist in the KDF11, MTPD executes in the same way 
// as MTPI does. 
//
class MTPD : public SingleOperandInstruction
{
public:
    MTPD (u16 instruction);
};

inline MTPD::MTPD (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _MTPD_H_