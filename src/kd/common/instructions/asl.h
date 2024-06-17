#ifndef _ASL_H_
#define _ASL_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// ASL - arithmetic shift left
//
// Operation:
//  (dst) <- (dst) shifted one place to the left
//
// Condition Codes:
//  N: set if high-order bit of the result is set (result < 0); cleared
//     otherwise
//  Z: set if the result = O; cleared otherwise
//  V: loaded with the exclusive OR of the N-bit and C-bit (as set by the
//     completion of the shift operation)
//  C: loaded with the high-order bit of the destination
//
// Shifts all bits of the destination left one place. Bit 0 is loaded with
// an 0. The C-bit of the status word is loaded from the most significant bit
// of the destination. ASL performs a signed multiplication of the destination
// by 2 with overflow indication.
//
class ASL : public SingleOperandInstruction
{
public:
    ASL (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
};

inline ASL::ASL (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

#endif // _ASL_H_