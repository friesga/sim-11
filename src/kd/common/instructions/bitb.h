#ifndef _BITB_H_
#define _BITB_H_


#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// BITB - bit test byte
//
// Operation:
//  refer to BIT
// 
// Condition Codes:
//  refer to BIT
//
class BITB : public DoubleOperandInstruction
{
public:
    BITB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
};

inline BITB::BITB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

#endif // _BITB_H_