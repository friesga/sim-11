#ifndef _MFPD_H_
#define _MFPD_H_

#include "proc/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "proc/include/cpudata.h"
#include "proc/include/cpucontroller.h"
#include "proc/common/operandlocation/operandlocation.h"


// MFPD - Move from previous data space
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
// Since data space does not exist in the KDFll, MFPD executes in the same way
// as an MFPI does. 
//
class MFPD : public SingleOperandInstruction
{
public:
    MFPD (u16 instruction);
};

inline MFPD::MFPD (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}

#endif // _MFPD_H_