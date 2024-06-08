#ifndef _MFPS_H_
#define _MFPS_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// MFPS - Move byte from Processor Status Word
//
// Operation:
//  (dst) < PSW
//  dst lower 8 bits
//
// Condition Codes:
//  N = set if PSW bit 7 = 1; cleared otherwise
//  Z = set if PS <0:7> = O; cleared otherwise·
//  V = cleared
//  C = not affected
// 
// The 8 bit contents of the PS are moved to the effective destination. If
// destination is mode 0, PS bit 7 is sign extended through upper byte of the
// register. The destination operand address is treated as a byte address.
//
class CommonInstruction::MFPS : public SingleOperandInstruction
{
public:
    MFPS (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::MFPS::MFPS (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::MFPS::execute ()
{
    throw "Should not happen";
}

#endif // _MFPS_H_