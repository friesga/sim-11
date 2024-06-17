#ifndef _ASLB_H_
#define _ASLB_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// ASLB - arithmetic shift left byte
//
// Operation:
//  refer to ASL
// 
// Condition Codes:
//  refer to ASL
//
// Same as ASL instruction with the distinction that for odd adresses the
// carry bit is loaded from bit 15 of the word and bit 8 is loaded with a zero
// and for even addresses the carry bit is loaded from bit 7 and bit 0 is
// loaded with a zero.
//
class ASLB : public SingleOperandInstruction
{
public:
    ASLB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
};

inline ASLB::ASLB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

#endif // _ASLB_H_