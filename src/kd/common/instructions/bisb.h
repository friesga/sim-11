#ifndef _BISB_H_
#define _BISB_H_


#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// BISB - bit set byte
//
// Operation:
//  refer to BIS
// 
// Condition Codes:
//  refer to BIS
//
class BISB : public DoubleOperandInstruction
{
public:
    BISB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
};

inline BISB::BISB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

#endif // _BISB_H_