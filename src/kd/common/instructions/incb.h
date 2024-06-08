#ifndef _INCB_H_
#define _INCB_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// INCB - increment destination byte
//
// Operation:
//  refer to INC
// 
// Condition Codes:
//  refer to INC
//
class CommonInstruction::INCB : public SingleOperandInstruction
{
public:
    INCB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::INCB::INCB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::INCB::execute ()
{
    throw "Should not happen";
}

#endif // _INCB_H_