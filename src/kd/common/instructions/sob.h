#ifndef _SOB_H_
#define _SOB_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/sobinstruction/sobinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// SOB - subtract one and branch (if != 0)
//
// Operation:
//  (R) <- (R) - 1; if this result != 0 then PC <- PC - (2 * offset)
//  if (R) = O; PC <- PC
//
// Condition Codes: unaffected
//
// The register is decremented. If it is not equal to 0, twice the offset is
// subtracted from the PC (now pointing to the following word). The offset is
// interpreted as a sixbit positive number.
//
class CommonInstruction::SOB : public SobInstruction
{
public:
    SOB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::SOB::SOB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SobInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::SOB::execute ()
{
    throw "Should not happen";
}

#endif // _SOB_H_