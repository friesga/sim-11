#include "kd11cpu.h"
#include "sobinstruction/sobinstruction.h"

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
void KD11CPU::SOB (KD11CPU *cpu, u16 (&reg)[8], u16 instruction)
{
    SobInstruction sobInstruction (cpu, instruction);
    u16 regNr = sobInstruction.getRegisterNr ();
    u16 offset = sobInstruction.getOffset ();

    reg[regNr]--;
    if (reg[regNr])
        reg[7] -= 2 * offset;
}