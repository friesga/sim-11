#include "kd11cpu.h"
#include "trace/trace.h"
#include "markinstruction/markinstruction.h"

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
void KD11CPU::MARK (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    MarkInstruction mi {cpu, instruction};

    register_[6] = register_[7] + 2 * mi.numberOfParameters ();
    register_[7] = register_[5];
    register_[5] = fetchWord (register_[6]);
    register_[6] += 2;
}