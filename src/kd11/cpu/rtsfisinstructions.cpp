#include "kd11cpu.h"
#include "arithmeticinstruction/arithmeticinstruction.h"

// Pop a word from the processor stack returning true if this succeeds
// or false when a bus error occurs.
bool KD11CPU::popWord (u16 *destination)
{
    CondData<u16> tmpValue = fetchWord (register_[6]);
    *destination = tmpValue.valueOr (0);
    register_[6] += 2;
    if (!tmpValue.hasValue ())
        return false;
    return true;
}

// RTS - Return from subroutine
// 
// Operation:
//  PC <- (reg)
//  (reg) <- (SP)^
// 
// Loads contents of reg into PC and pops the top element of
// the processor stack into the specified register.
//
void KD11CPU::RTS (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    ArithmeticInstruction rtsInstruction (cpu, instruction);
    u16 regNr = rtsInstruction.getRegister ();

    register_[7] = register_[regNr];
    popWord (&register_[regNr]);
}