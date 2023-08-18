#include "doubleoperandinstruction/doubleoperandinstruction.h"

#include "bitmask.h"

/*
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
void KD11CPU::MOV (CpuData* cpu, u16 instruction)
{
    DoubleOperandInstruction doubleOperandInstruction (cpu, instruction);

    OperandLocation sourceOperandLocation = 
            doubleOperandInstruction.getSourceOperandLocation (cpu->registers ());
    CondData<u16> source = sourceOperandLocation.wordContents ();
    if (!source.hasValue ())
        return;

    OperandLocation destOperandLocation =
        doubleOperandInstruction.getDestinationOperandLocation (cpu->registers ());

    destOperandLocation.write (source);

    setConditionCodeIf_ClearElse (PSW_N, source & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !source);
    clearConditionCode (PSW_V);
}
*/

/*
// MOVB - move source to destination
// 
// Operation:
//  refer to MOV
// 
// Condition Codes:
//  refer to MOV
//
// The MOVB to a register (unique among byte instructions) extends the most
// significant bit of the low order byte (sign extension). Otherwise MOVB
// operates on bytes exactly as MOV operates on words.
//
void KD11CPU::MOVB (CpuData* cpu, u16 instruction)
{
    DoubleOperandInstruction doubleOperandInstruction (cpu, instruction);

    OperandLocation sourceOperandLocation =
            doubleOperandInstruction.getSourceOperandLocation (cpu->registers ());
    CondData<u8> source = sourceOperandLocation.byteContents ();
    if (!source.hasValue ())
        return;

    s8 tmp = (s8) source;

    OperandLocation destOperandLocation =
            doubleOperandInstruction.getDestinationOperandLocation (cpu->registers ());

    // If the destination mode is 0 (Register) the regular operand processing
    // is bypassed and the signed eight bit value u8 is directly written into
    // the register, causing sign extension in the register.
    if (destOperandLocation.isA<u8> ())
        register_[destOperandLocation] = tmp;
    else
        if (!destOperandLocation.writeByte (tmp))
            return;
    
    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
    clearConditionCode (PSW_V);
}
*/

/*
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
void KD11CPU::CMP (CpuData* cpu, u16 instruction)
{
    DoubleOperandInstruction doubleOperandInstruction (cpu, instruction);

    OperandLocation sourceOperandLocation = 
            doubleOperandInstruction.getSourceOperandLocation (cpu->registers ());
    CondData<u16> source = sourceOperandLocation.wordContents ();
    if (!source.hasValue ())
        return;

    OperandLocation destinationOperandLocation = 
            doubleOperandInstruction.getDestinationOperandLocation (cpu->registers ());
    CondData<u16> destination = destinationOperandLocation.wordContents ();
    if (!destination.hasValue ())
        return;

    u16 tmp = source - destination;
    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x8000);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
    setConditionCodeIf_ClearElse (PSW_V, ((source & 0x8000) != (destination & 0x8000)) \
        && ((destination & 0x8000) == (tmp & 0x8000)));
    setConditionCodeIf_ClearElse (PSW_C, ((u32)source - (u32)destination) & 0x10000);
}
*/

/*
// CMPB - compare source to destination byte
// 
// Operation:
//  refer to CMP
// 
// Condition Codes:
//  refer to CMP
//
void KD11CPU::CMPB (CpuData* cpu, u16 instruction)
{
    DoubleOperandInstruction cmpbInstruction (cpu, instruction);

    OperandLocation sourceOperandLocation =
            cmpbInstruction.getSourceOperandLocation (cpu->registers ());
    CondData<u8> source = sourceOperandLocation.byteContents ();
    if (!source.hasValue ())
        return;

    OperandLocation destOperandLocation =
            cmpbInstruction.getDestinationOperandLocation (cpu->registers ());
    CondData<u8> destination = destOperandLocation.byteContents ();
    if (!destination.hasValue ())
        return;

    u16 tmp = (u8) (source - destination);

    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
    setConditionCodeIf_ClearElse (PSW_V, ((source & 0x80) != (destination & 0x80)) \
        && ((destination & 0x80) == (tmp & 0x80)));
    setConditionCodeIf_ClearElse (PSW_C, (source - destination) & 0x100);
}
*/

/*
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
void KD11CPU::BIT (CpuData* cpu, u16 instruction)
{
    DoubleOperandInstruction doubleOperandInstruction (cpu, instruction);

    OperandLocation sourceOperandLocation = 
            doubleOperandInstruction.getSourceOperandLocation (cpu->registers ());
    CondData<u16> source = sourceOperandLocation.wordContents ();
    if (!source.hasValue ())
        return;

    OperandLocation destinationOperandLocation = 
            doubleOperandInstruction.getDestinationOperandLocation (cpu->registers ());
    CondData<u16> destination = destinationOperandLocation.wordContents ();
    if (!destination.hasValue ())
        return;

    u16 tmp = source & destination;
    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x8000);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
    clearConditionCode (PSW_V);
}
*/

/*
// BITB - bit test byte
//
// Operation:
//  refer to BIT
// 
// Condition Codes:
//  refer to BIT
//
void KD11CPU::BITB (CpuData* cpu, u16 instruction)
{
    DoubleOperandInstruction bitbInstruction (cpu, instruction);

    OperandLocation sourceOperandLocation =
            bitbInstruction.getSourceOperandLocation (cpu->registers ());
    CondData<u8> source = sourceOperandLocation.byteContents ();
    if (!source.hasValue ())
        return;

    OperandLocation destOperandLocation =
            bitbInstruction.getDestinationOperandLocation (cpu->registers ());
    CondData<u8> destination = destOperandLocation.byteContents ();
    if (!destination.hasValue ())
        return;

    u16 tmp = source & destination;
    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
    clearConditionCode (PSW_V);
}
*/

/*
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
void KD11CPU::BIC (CpuData* cpu, u16 instruction)
{
    DoubleOperandInstruction doubleOperandInstruction (cpu, instruction);

    OperandLocation sourceOperandLocation = 
            doubleOperandInstruction.getSourceOperandLocation (cpu->registers ());
    CondData<u16> source = sourceOperandLocation.wordContents ();
    if (!source.hasValue ())
        return;

    OperandLocation destinationOperandLocation = 
            doubleOperandInstruction.getDestinationOperandLocation (cpu->registers ());
    CondData<u16> destination = destinationOperandLocation.wordContents ();
    if (!destination.hasValue ())
        return;

    u16 tmp = ~source & destination;

    destinationOperandLocation.write (tmp);

    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x8000);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
    clearConditionCode (PSW_V);
}
*/

/*
// BICB - bit clear byte
//
// Operation:
//  refer to BIC
// 
// Condition Codes:
//  refer to BIC
//
void KD11CPU::BICB (CpuData* cpu, u16 instruction)
{
    DoubleOperandInstruction bicbInstruction (cpu, instruction);

    OperandLocation sourceOperandLocation =
            bicbInstruction.getSourceOperandLocation (cpu->registers ());
    CondData<u8> source = sourceOperandLocation.byteContents ();
    if (!source.hasValue ())
        return;

    OperandLocation destOperandLocation =
            bicbInstruction.getDestinationOperandLocation (cpu->registers ());
    CondData<u8> destination = destOperandLocation.byteContents ();
    if (!destination.hasValue ())
        return;

    u16 tmp = (u8) (~source & destination);

    if (!destOperandLocation.writeByte (tmp))
        return;

    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
    clearConditionCode (PSW_V);
}
*/

/*
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
void KD11CPU::BIS (CpuData* cpu, u16 instruction)
{
    DoubleOperandInstruction doubleOperandInstruction (cpu, instruction);

    OperandLocation sourceOperandLocation = 
            doubleOperandInstruction.getSourceOperandLocation (cpu->registers ());
    CondData<u16> source = sourceOperandLocation.wordContents ();
    if (!source.hasValue ())
        return;

    OperandLocation destinationOperandLocation = 
            doubleOperandInstruction.getDestinationOperandLocation (cpu->registers ());
    CondData<u16> destination = destinationOperandLocation.wordContents ();
    if (!destination.hasValue ())
        return;

    u16 tmp = source | destination;

    destinationOperandLocation.write (tmp);

    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x8000);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
    clearConditionCode (PSW_V);
}
*/

// BISB - bit set byte
//
// Operation:
//  refer to BIS
// 
// Condition Codes:
//  refer to BIS
//
void KD11CPU::BISB (CpuData* cpu, u16 instruction)
{
    DoubleOperandInstruction bisbInstruction (cpu, instruction);

    OperandLocation sourceOperandLocation =
            bisbInstruction.getSourceOperandLocation (cpu->registers ());
    CondData<u8> source = sourceOperandLocation.byteContents ();
    if (!source.hasValue ())
        return;

    OperandLocation destOperandLocation =
            bisbInstruction.getDestinationOperandLocation (cpu->registers ());
    CondData<u8> destination = destOperandLocation.byteContents ();
    if (!destination.hasValue ())
        return;

    u16 tmp = source | destination;

    if (!destOperandLocation.writeByte (tmp))
        return;

    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
    clearConditionCode (PSW_V);
}

/*
// ADD - add source to destination
//
// Operation:
//  (dst) <- (src) + (dst)
//
// Condition Codes:
//  N: set if result <0; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: set if there was arithmetic overflow as a result of the operation;
//     that is both operands were of the same sign and the result was of the
//     opposite sign; cleared otherwise
//  C: set if there was a carry from the most significant bit of the result;
//     cleared otherwise
//
// Adds the source operand to the destinatign operand and stores the result at
// the destination address. The original contents of the destination are lost.
// The contents of the source are not affected. Two's complement addition is
// performed.
//
void KD11CPU::ADD (CpuData* cpu, u16 instruction)
{
    DoubleOperandInstruction doubleOperandInstruction (cpu, instruction);

    OperandLocation sourceOperandLocation = 
            doubleOperandInstruction.getSourceOperandLocation (cpu->registers ());
    CondData<u16> source = sourceOperandLocation.wordContents ();
    if (!source.hasValue ())
        return;

    OperandLocation destinationOperandLocation = 
            doubleOperandInstruction.getDestinationOperandLocation (cpu->registers ());
    CondData<u16> destination = destinationOperandLocation.wordContents ();
    if (!destination.hasValue ())
        return;

    u16 result = source + destination;

    destinationOperandLocation.write (result);

    setConditionCodeIf_ClearElse (PSW_N, result & 0x8000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    setConditionCodeIf_ClearElse (PSW_V, ((source & 0x8000) == (destination & 0x8000)) \
        && ((destination & 0x8000) != (result & 0x8000)));
    setConditionCodeIf_ClearElse (PSW_C, ((u32)source + (u32)destination) & 0x10000);
}
*/

/*
// SUB - subtract src from dst
//
// Operation:
//  (dst) <- (dst) - (src)
//
// Condition Codes:
//  N: set if result <0; cleared otherwise
//  Z: set if result = 0; cleared otherwise
//  V: set if there was arithmetic overflow as a result of the operation,
//     that is if operands were of opposite signs and the sign of the source
//     was the same as the sign of the result; cleared otherwise
//  C: cleared if there was a carry from the most significant bit of the
//     result; set otherwise
//
void KD11CPU::SUB (CpuData* cpu, u16 instruction)
{
    DoubleOperandInstruction subInstruction (cpu, instruction);

    OperandLocation sourceOperandLocation = 
            subInstruction.getSourceOperandLocation (cpu->registers ());
    CondData<u16> source = sourceOperandLocation.wordContents ();
    if (!source.hasValue ())
        return;

    OperandLocation destinationOperandLocation = 
            subInstruction.getDestinationOperandLocation (cpu->registers ());
    CondData<u16> destination = destinationOperandLocation.wordContents ();
    if (!destination.hasValue ())
        return;

    u16 result = destination - source;

    if (!destinationOperandLocation.write (result))
        return;

    setConditionCodeIf_ClearElse (PSW_N, result & 0x8000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    setConditionCodeIf_ClearElse (PSW_V, ((source & 0x8000) != (destination & 0x8000)) \
        && ((source & 0x8000) == (result & 0x8000)));
    setConditionCodeIf_ClearElse (PSW_C, ((u32) destination - (u32) source) & 0x10000);
}
*/