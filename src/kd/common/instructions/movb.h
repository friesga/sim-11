#ifndef _MOVB_H_
#define _MOVB_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// MOVB - move source to destination
// 
// Operation:
//  refer to MOV
// 
// Condition Codes:
//  refer to MOV
//
// The MOVB to a register (unique among byte instructions) extends the most
// significant bit of the low order byte (sign extension). Otherwise MOVB
// operates on bytes exactly as MOV operates on words.
//
class CommonInstruction::MOVB : public DoubleOperandInstruction
{
public:
    MOVB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::MOVB::MOVB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::MOVB::execute ()
{
    throw "Should not happen";
}

#endif // _MOVB_H_