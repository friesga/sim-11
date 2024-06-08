#ifndef _CLR_H_
#define _CLR_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// CLR - clear destination
//
// Operation:
//  (dst) <- 0
//
// Condition Codes:
//  N: cleared
//  Z: set
//  V: cleared
//  C: cleared
//
// Contents of specified destination are replaced with zeroes.
//
class CommonInstruction::CLR : public SingleOperandInstruction
{
public:
    CLR (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::CLR::CLR (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::CLR::execute ()
{
    throw "Should not happen";
}

#endif // _CLR_H_