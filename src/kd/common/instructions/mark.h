#ifndef _MARK_H_
#define _MARK_H_


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
class MARK : public MarkInstruction
{
public:
    MARK (u16 instruction);
};

inline MARK::MARK (u16 instruction)
    :
    MarkInstruction (instruction)
{}

#endif // _MARK_H_