#ifndef _NEGB_H_
#define _NEGB_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// NEGB - negate destination byte
//
// Operation:
//  refer to NEG
// 
// Condition Codes:
//  refer to NEG
//
class CommonInstruction::NEGB : public SingleOperandInstruction
{
public:
    NEGB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::NEGB::NEGB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::NEGB::execute ()
{
    throw "Should not happen";
}

#endif // _NEGB_H_