#ifndef _DIV_H_
#define _DIV_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/eisinstruction/eisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// DIV - divide
//
// Operation:
//  R,R+1 <- R,R+1 / (src)
//
// Condition Codes:
//  N: set if quotient <0: cleared otherwise
//  Z: set if quotient = 0: cleared otherwise
//  V: set if source = 0 or if the absolute value of the register is larger
//     than the absolute value of the source. (In this case the instruction
//     is aborted because the quotient would exceed 15 bits.)
//  C: set if divide 0 attempted; cleared otherwise
//
// The 32-bit two's complement integer in R and R+1 is divided by the source
// operand. The quotient is left in R: the remainder in R+1. Division will be
// performed so that the remainder is of the same sign as the dividend. R must
// be even.
//
class CommonInstruction::DIV : public EisInstruction
{
public:
    DIV (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::DIV::DIV (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    EisInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: Should not happen
inline bool CommonInstruction::DIV::execute ()
{
    throw "Should not happen";
}

#endif // _DIV_H_