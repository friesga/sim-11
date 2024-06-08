#ifndef _ASHC_H_
#define _ASHC_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/eisinstruction/eisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// ASHC - arithemetic shift combined
//
// Operation:
//  R, R+1 <- R, R+1 The double word is shifted NN places to the right or
//  left, where NN =low order six bits of source
//
// Condition Codes:
//  N: set if result <0; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: set if sign bit changes during the shift; cleared otherwise
//  C: loaded with high order bit when left shift; loaded with low order bit
//     when right shift (loaded with the last bit shifted out of the 32-bit
//     operand)
//
// The contents of the register and the register ORed with one are treated as
// one 32 bit word, R + 1 (bits 0-15) and R (bits 16-31) are shifted right or
// left the number of times specified by the shift count. The shift count is
// taken as the low order 6 bits of the source operand. This number ranges
// from -32 to +31. Negative is a right shift and positive is a left shift.
// 
// When the register chosen is an odd number the register and the register
// OR'ed with one are the same. In this case the right shift becomes a rotate
// (for up to a shift of 16). The 16 bit word is rotated right the number of
// bits specified by the shift count.
//
class CommonInstruction::ASHC : public EisInstruction
{
public:
    ASHC (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::ASHC::ASHC (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    EisInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::ASHC::execute ()
{
    throw "Should not happen";
}

#endif // _ASHC_H_