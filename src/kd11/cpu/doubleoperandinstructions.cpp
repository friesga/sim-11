#include "doubleoperandinstruction/doubleoperandinstruction.h"

#include "kd11insn2/kd11insn2.h"
#include "bitmask.h"

// MOV - move source to destination
//
// Operation:
//  (dst) <- (src)
//
// Condition Codes:
//  N: set if (src) < 0: cleared otherwise
//  Z: set if (src) = 0: cleared otherwise
//  V: cleared
//  C: not affected
//
// Moves the source operand to the destination location. The previous
// contents of the destination are lost. The contents of the source address
// are not affected.
//
void KD11CPU::MOV (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    DoubleOperandInstruction doubleOperandInstruction (cpu, instruction);

    OperandLocation sourceOperandLocation = 
            doubleOperandInstruction.getSourceOperandLocation (reg);
    CondData<u16> source = sourceOperandLocation.contents ();
    if (!source.hasValue ())
        return;

    OperandLocation destOperandLocation =
        doubleOperandInstruction.getDestinationOperandLocation (reg);

    destOperandLocation.write (source);

    PSW_EQ (PSW_N, source & 0100000);
    PSW_EQ (PSW_Z, !source);
    PSW_CLR (PSW_V);
}