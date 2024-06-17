#ifndef _CMP_H_
#define _CMP_H_


#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// CMP - compare source to destination
//
// Operation:
//  (src)=(dst)
//
// Condition Codes:
//  N: set if result <0; cleared otherwise
//  Z: set if result = 0; cleared otherwise
//  V: set if there was arithmetic overflow; that is. operands were of
//     opposite signs and the sign of the destination was the same as the
//     sign of the result: cleared otherwise
//  C: cleared if there was a carry from the most significant bit of the
//     result; set otherwise
//
// Compares the source and destination operands and sets the condition codes,
// which may then be used for arithmetic and logical conditional branches.
// Both operands are unaffected. The only action is to set the condition codes.
//
class CMP : public DoubleOperandInstruction
{
public:
    CMP (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
};

inline CMP::CMP (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

#endif // _CMP_H_