#ifndef _ROLB_H_
#define _ROLB_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// ROLB - rotate left byte
//
// Operation:
//  refer to ROL
// 
// Condition Codes:
//  refer to ROL
//
// Same as ROL instruction with the distinction that for odd adresses the
// carry bit is loaded with bit 15 of the word and for even addresses the carry
// bit is loaded with bit 7 of the word.
//
class CommonInstruction::ROLB : public SingleOperandInstruction
{
public:
    ROLB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::ROLB::ROLB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::ROLB::execute ()
{
    throw "Should not happen";
}

#endif // _ROLB_H_