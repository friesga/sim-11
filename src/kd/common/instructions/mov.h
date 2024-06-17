#ifndef _MOV_H_
#define _MOV_H_


#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// MOV - move source to destination
//
// Operation:
//  (dst) <- (src)
//
// Condition Codes:
//  N: set if (src) < 0: cleared otherwise
//  Z: set if (src) = 0: cleared otherwise
//  V: cleared
//  C: not affected
//
// Moves the source operand to the destination location. The previous
// contents of the destination are lost. The contents of the source address
// are not affected.
//
class MOV : public DoubleOperandInstruction
{
public:
    MOV (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
};

inline MOV::MOV (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

#endif // _MOV_H_