#ifndef _BICB_H_
#define _BICB_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// BICB - bit clear byte
//
// Operation:
//  refer to BIC
// 
// Condition Codes:
//  refer to BIC
//
class CommonInstruction::BICB : public DoubleOperandInstruction
{
public:
    BICB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::BICB::BICB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::BICB::execute ()
{
    throw "Should not happen";
}

#endif // _BICB_H_