#ifndef _DEC_H_
#define _DEC_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// DEC - decrement destination
//
// Operation:
//  (dst) <- (dst) - 1
//
// Condition Codes:
//  N: set if result is <0; cleared otherwise
//  Z: set if result is 0; cleared otherwise
//  V: set if (dst) was 100000; cleared otherwise
//  C: not affected
//
// Subtract 1 from the contents of the destination
//
class CommonInstruction::DEC : public SingleOperandInstruction
{
public:
    DEC (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::DEC::DEC (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::DEC::execute ()
{
    throw "Should not happen";
}

#endif // _DEC_H_