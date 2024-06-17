#ifndef _CCC_H_
#define _CCC_H_


#include "kd/common/instructionformats/ccinstruction/ccinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


//
// Clear condition code bits. Selectable combinations of these bits may
// be cleared together. Condition code bits in the PSW corresponding to bits
// in the condition code operator (bits O-3) are cleared.
// 
// The operation code 0240 is documented as the NOP (no-operation)
// instruction. The operation code indicates this a CCC instruction without
// any condition code set and thus the instruction has no effect.
// 
class CCC : public CcInstruction
{
public:
    CCC (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
};

inline CCC::CCC (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    CcInstruction (cpuData, cpuControl, mmu, instruction)
{}

#endif // _CCC_H_