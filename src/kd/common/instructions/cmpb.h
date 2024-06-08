#ifndef _CMPB_H_
#define _CMPB_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// CMPB - compare source to destination byte
// 
// Operation:
//  refer to CMP
// 
// Condition Codes:
//  refer to CMP
//
class CommonInstruction::CMPB : public DoubleOperandInstruction
{
public:
    CMPB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::CMPB::CMPB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::CMPB::execute ()
{
    throw "Should not happen";
}

#endif // _CMPB_H_