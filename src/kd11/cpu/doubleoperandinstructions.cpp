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

// CMP - compare source to destination
//
// Operation:
//  (src)=(dst)
//
// Condition Codes:
//  N: set if result <0; cleared otherwise
//  Z: set if result = 0; cleared otherwise
//  V: set if there was arithmetic overflow; that is. operands were of
//     opposite signs and the sign of the destination was the same as the
//     sign of the result: cleared otherwise
//  C: cleared if there was a carry from the most significant bit of the
//     result; set otherwise
//
// Compares the source and destination operands and sets the condition codes,
// which may then be used for arithmetic and logical conditional branches.
// Both operands are unaffected. The only action is to set the condition codes.
//
void KD11CPU::CMP (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    DoubleOperandInstruction doubleOperandInstruction (cpu, instruction);

    OperandLocation sourceOperandLocation = 
            doubleOperandInstruction.getSourceOperandLocation (reg);
    CondData<u16> source = sourceOperandLocation.contents ();
    if (!source.hasValue ())
        return;

    OperandLocation destinationOperandLocation = 
            doubleOperandInstruction.getDestinationOperandLocation (reg);
    CondData<u16> destination = destinationOperandLocation.contents ();
    if (!destination.hasValue ())
        return;

    u16 tmp = source - destination;
    PSW_EQ (PSW_N, tmp & 0x8000);
    PSW_EQ (PSW_Z, !tmp);
    PSW_EQ (PSW_V, ((source & 0x8000) != (destination & 0x8000)) \
        && ((destination & 0x8000) == (tmp & 0x8000)));
    PSW_EQ (PSW_C, ((u32)source - (u32)destination) & 0x10000);
}

// BIT - bit test
//
// Operation:
//  (src) ^ (dst)
//
// Condition Codes:
//  N: set if high-order bit of result set: cleared otherwise
//  Z: set if result = O: cleared otherwise
//  V: cleared
//  C: not affected
//
// Performs logical "and"comparison of the source and destination operands
// and modifies condition codes accordingly. Neither the source nor
// destination operands are affected.
//
void KD11CPU::BIT (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    DoubleOperandInstruction doubleOperandInstruction (cpu, instruction);

    OperandLocation sourceOperandLocation = 
            doubleOperandInstruction.getSourceOperandLocation (reg);
    CondData<u16> source = sourceOperandLocation.contents ();
    if (!source.hasValue ())
        return;

    OperandLocation destinationOperandLocation = 
            doubleOperandInstruction.getDestinationOperandLocation (reg);
    CondData<u16> destination = destinationOperandLocation.contents ();
    if (!destination.hasValue ())
        return;

    u16 tmp = source & destination;
    PSW_EQ (PSW_N, tmp & 0x8000);
    PSW_EQ (PSW_Z, !tmp);
    PSW_CLR (PSW_V);
}

// BIC - bit clear
//
// Operation:
//  (dst) <- (src) ^ (dst)
//
// Condition Codes:
//  N: set if high order bit of result set; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: cleared
//  C: not affected
//
// Clears each bit in the destination that corresponds to a set bit in the
// source. The original contents of the destination are lost. The contents of
// the source are unaffected.
//
void KD11CPU::BIC (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    DoubleOperandInstruction doubleOperandInstruction (cpu, instruction);

    OperandLocation sourceOperandLocation = 
            doubleOperandInstruction.getSourceOperandLocation (reg);
    CondData<u16> source = sourceOperandLocation.contents ();
    if (!source.hasValue ())
        return;

    OperandLocation destinationOperandLocation = 
            doubleOperandInstruction.getDestinationOperandLocation (reg);
    CondData<u16> destination = destinationOperandLocation.contents ();
    if (!destination.hasValue ())
        return;

    u16 tmp = ~source & destination;

    destinationOperandLocation.write (tmp);

    PSW_EQ (PSW_N, tmp & 0x8000);
    PSW_EQ (PSW_Z, !tmp);
    PSW_CLR (PSW_V);
}

// BIS - bit set
//
// Operation:
//  (dst) <- (src) v (dst)
//
// Condition Codes:
//  N: set if high-order bit of result set. cleared otherwise
//  Z: set if result = O: cleared otherwise
//  V: cleared
//  C: not affected
//
// Performs "Inclusive OR"operation between the source and destination
// operands and leaves the result at the destination address: that is,
// corresponding bits set in the source are set in the destination. The
// contents of the destination are lost.
//
void KD11CPU::BIS (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    DoubleOperandInstruction doubleOperandInstruction (cpu, instruction);

    OperandLocation sourceOperandLocation = 
            doubleOperandInstruction.getSourceOperandLocation (reg);
    CondData<u16> source = sourceOperandLocation.contents ();
    if (!source.hasValue ())
        return;

    OperandLocation destinationOperandLocation = 
            doubleOperandInstruction.getDestinationOperandLocation (reg);
    CondData<u16> destination = destinationOperandLocation.contents ();
    if (!destination.hasValue ())
        return;

    u16 tmp = source | destination;

    destinationOperandLocation.write (tmp);

    PSW_EQ (PSW_N, tmp & 0x8000);
    PSW_EQ (PSW_Z, !tmp);
    PSW_CLR (PSW_V);
}