#ifndef _ADCB_H_
#define _ADCB_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// ADCB - add carry byte
//
// Operation:
//  refer to ADC
// 
// Condition Codes:
//  refer to ADC
//
class CommonInstruction::ADCB : public SingleOperandInstruction
{
public:
    ADCB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::ADCB::ADCB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::ADCB::execute ()
{
    throw "Should not happen";
}

#endif // _ADCB_H_