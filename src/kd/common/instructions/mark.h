#ifndef _MARK_H_
#define _MARK_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/markinstruction/markinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// MARK - mark
//
// Operation:
//  SP <- updated PC + 2 + 2n n = number of parameters
//  PC <- R5
//  R5 <- (SP)^
//
// Condition Codes: unaffected
//
// MARK n causes:
//  (1) the stack pointer to be adjusted to point to the old R5 value;
//  (2) the value now in R5 (the old PC) to be placed in the PC; and
//  (3) contents of the old R5 to be popped into R5 thus completing the
//      return from subroutine.
//
class CommonInstruction::MARK : public MarkInstruction
{
public:
    MARK (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::MARK::MARK (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    MarkInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::MARK::execute ()
{
    throw "Should not happen";
}

#endif // _MARK_H_