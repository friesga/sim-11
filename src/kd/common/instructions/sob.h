#ifndef _SOB_H_
#define _SOB_H_


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
class SOB : public SobInstruction
{
public:
    SOB (u16 instruction);
};

inline SOB::SOB (u16 instruction)
    :
    SobInstruction (instruction)
{}

#endif // _SOB_H_