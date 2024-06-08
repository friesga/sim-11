#ifndef _MUL_H_
#define _MUL_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/eisinstruction/eisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// MUL - multiply
//
// Operation:
//  R, R+1 <- R x (src)
//
//  Condition Codes:
//  N: set if product is <0: cleared otherwise
//  Z: set if product is O: cleared otherwise
//  V: cleared
//  C: set if the result is less than-215 or greater than or equal to 2^15 - 1
//
// The contents of the destination register and source taken as two's complement
// integers are multiplied and stored in the destination register and the
// succeeding register (if R is even). If R is odd only the low order product
// is stored.
//
class CommonInstruction::MUL : public EisInstruction
{
public:
    MUL (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::MUL::MUL (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    EisInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::MUL::execute ()
{
    throw "Should not happen";
}

#endif // _MUL_H_