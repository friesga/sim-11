#ifndef _XOR_H_
#define _XOR_H_


#include "kd/common/instructionformats/eisinstruction/eisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// XOR - exclusive OR
//
// Operation:
//  (dst) <- R v (dst)
//
// Condition Codes:
//  N: set if the result <0: cleared otherwise
//  Z: set if result = O: cleared otherwise
//  V: cleared
//  C: unaffected
//
// The exclusive OR of the register and destination operand is stored in the
// destination address. Contents of register are unaffected.
//
class XOR : public EisInstruction
{
public:
    XOR (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
};

inline XOR::XOR (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    EisInstruction (cpuData, cpuControl, mmu, instruction)
{}

#endif // _XOR_H_