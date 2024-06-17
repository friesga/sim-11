#ifndef _SUB_H_
#define _SUB_H_


#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// SUB - subtract src from dst
//
// Operation:
//  (dst) <- (dst) - (src)
//
// Condition Codes:
//  N: set if result <0; cleared otherwise
//  Z: set if result = 0; cleared otherwise
//  V: set if there was arithmetic overflow as a result of the operation,
//     that is if operands were of opposite signs and the sign of the source
//     was the same as the sign of the result; cleared otherwise
//  C: cleared if there was a carry from the most significant bit of the
//     result; set otherwise
//
class SUB : public DoubleOperandInstruction
{
public:
    SUB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
};

inline SUB::SUB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

#endif // _SUB_H_