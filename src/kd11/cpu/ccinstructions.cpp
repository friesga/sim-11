#include "kd11cpu.h"
#include "ccinstruction/ccinstruction.h"

// Clear condition code bits. Selectable combinations of these bits may
// be cleared together. Condition code bits in the PSW corresponding to bits
// in the condition code operator (bits O-3) are cleared.
// 
void KD11CPU::CCC (u16 &psw, u16 instruction)
{
    CcInstruction ccInstruction {instruction};
    psw &= ~ccInstruction.getConditionCodes ();
}

// Set condition code bits. Selectable combinations of these bits may
// be set together. Condition code bits in the PSW corresponding to bits
// in the condition code operator (bits O-3) are set.
// 
void KD11CPU::SCC (u16 &psw, u16 instruction)
{
    CcInstruction ccInstruction {instruction};
    psw |= ccInstruction.getConditionCodes ();
}