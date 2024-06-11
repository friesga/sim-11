#ifndef _ASH_H_
#define _ASH_H_


#include "kd/common/instructionformats/eisinstruction/eisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// ASH - shift arithmetically
//
// Operation:
//  R <- R shifted arithmetically NN places to right or left where
//  NN = low order 6 bits of source
//
// Condition Codes:
//  N: set if result <O; cleared otherwise
//  Z: set if result = 0; cleared otherwise
//  V: set if sign of register changed during shift; cleared otherwise
//  C: loaded from last bit shifted out of register
//
// The contents of the register are shifted right or left the number of times
// specified by the shift count. The shift count is taken as the low order
// 6 bits of the source operand. This number ranges from -32 to + 31. Negative
// is a a right shift and positive is a left shift.
//
class ASH : public EisInstruction
{
public:
    ASH (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline ASH::ASH (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    EisInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool ASH::execute ()
{
    throw "Should not happen";
}
#endif // _ASH_H_