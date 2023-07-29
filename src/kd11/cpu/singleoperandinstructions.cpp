#include "kd11cpu.h"
#include "trace/trace.h"
#include "singleoperandinstruction/singleoperandinstruction.h"

// JMP - jump
//
// Operation:
//  PC <- (dst)
//
// JMP provides more flexible program branching than provided with the branch
// instructions. Control may be transferred to any location in memory (no
// range limitation) and can be accomplished with the full flexibility of the
// addressing modes, with the exception of register mode 0. Execution of a
// jump with mode 0 will cause an "illegal instruction" condition, and will
// cause the CPU to trap to vector address 4. (Program control cannot be
// transferred to a register.) Register deferred mode is legal and will cause
// program control to be transferred to the address held in the specified
// register. Note that instructions are word data and must therefore be
// fetched from an even-numbered address.
void KD11CPU::JMP (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};
    OperandLocation location = soi.getOperandLocation (reg);

    if (location.isA<CondData<u16>> ())
        reg[7] = location;
    else
    {
        // Illegal instruction
        trace.trap (TrapRecordType::TRAP_RADDR, 04);
        setTrap (&busError);
    }
}

// SWAB - swap bytes
//
// Operation:
//  byte 1/byte 0 <- byte 0/byte 1
//
// Condition Codes:
//  N: set if high-order bit of low-order byte (bit 7) of result is set;
//     cleared otherwise
//  Z: set if low-order byte of result = O; cleared otherwise
//  V: cleared
//  C: cleared
// 
// Exchanges high-order byte and low-order byte of the destination
// word (destination must be a word address).
//
void KD11CPU::SWAB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (reg);
    CondData<u16> operand = operandLocation.wordContents ();
    if (!operand.hasValue ())
        return;

    // Swap bytes in the operand and write it to the operand location
    operand = ((operand & 0x00FF) << 8) | ((operand >> 8) & 0xFF);
    operandLocation.write (operand);

    PSW_EQ (PSW_N, operand & 0x80);
    PSW_EQ (PSW_Z, !((u8) operand));
    PSW_CLR (PSW_V);
    PSW_CLR (PSW_C);
}

// CLR - clear destination
//
// Operation:
//  (dst) <- 0
//
// Condition Codes:
//  N: cleared
//  Z: set
//  V: cleared
//  C: cleared
//
// Contents of specified destination are replaced with zeroes.
//
void KD11CPU::CLR (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (reg);
    operandLocation.write (0);

    PSW_CLR (PSW_N | PSW_V | PSW_C);
    PSW_SET (PSW_Z);
}

// CLRB - clear destination byte
//
// Operation:
//  refer to CLR
// 
// Condition Codes:
//  refer to CLR
//
void KD11CPU::CLRB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction clrbInstruction {cpu, instruction};
    OperandLocation operandLocation =
        clrbInstruction.getOperandLocation (reg);

    operandLocation.writeByte (0);

    PSW_CLR (PSW_N | PSW_V | PSW_C);
    PSW_SET (PSW_Z);
}

// COM - complement destination
//
// Operation:
//  (dst) <- ~(dst)
//
// Condition Codes:
//  N: set if most significant bit of result is set; cleared otherwise
//  Z: set if result is 0; cleared otherwise
//  V: cleared
//  C: set
//
// Replaces the contents of the destination address by their logical
// complement (each bit equal to 0 is set and each bit equal to 1 is cleared).
//
void KD11CPU::COM (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (reg);
    CondData<u16> operand = operandLocation.wordContents ();
    if (!operand.hasValue ())
        return;

    // Complement the operand and write it to the operand location
    operand = ~operand;
    operandLocation.write (operand);

    PSW_CLR (PSW_V);
    PSW_SET (PSW_C);
    PSW_EQ (PSW_N, operand & 0x8000);
    PSW_EQ (PSW_Z, !operand);
}

// COM - complement destination byte
//
// Operation:
//  refer to COM
// 
// Condition Codes:
//  refer to COM
//
void KD11CPU::COMB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction combInstruction {cpu, instruction};

    OperandLocation operandLocation = 
        combInstruction.getOperandLocation (reg);
    CondData<u8> operand = operandLocation.byteContents ();
    if (!operand.hasValue ())
        return;

    // Complement the operand and write it to the operand location
    operand = ~operand;
    operandLocation.writeByte (operand);

    PSW_CLR (PSW_V);
    PSW_SET (PSW_C);
    PSW_EQ (PSW_N, operand & 0x80);
    PSW_EQ (PSW_Z, !((u8) operand));
}

// INC - increment destination
//
// Operation:
//  (dst) <- (dst) + 1
//
// Condition Codes:
//  N: set if result is <0; cleared otherwise
//  Z: set if result is O; cleared otherwise
//  V: set if (dst) held 077777; cleared otherwise
//  C: not affected
//
// Add one to contents of destination
//
void KD11CPU::INC (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (reg);
    CondData<u16> contents = operandLocation.wordContents ();
    if (!contents.hasValue ())
        return;

    // Increment the operand and write it to the operand location
    u16 result = contents + 1;
    operandLocation.write (result);

    PSW_EQ (PSW_V, contents == 077777)
    PSW_EQ (PSW_N, result & 0x8000);
    PSW_EQ (PSW_Z, !result);
}

// INCB - increment destination byte
//
// Operation:
//  refer to INC
// 
// Condition Codes:
//  refer to INC
//
void KD11CPU::INCB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction incbInstruction {cpu, instruction};

    OperandLocation operandLocation = 
        incbInstruction.getOperandLocation (reg);
    CondData<u8> src = operandLocation.byteContents ();
    if (!src.hasValue ())
        return;

    u8 tmp = (u8) (src + 1);

    operandLocation.writeByte (tmp);

    PSW_EQ (PSW_V, src == 000177)
        PSW_EQ (PSW_N, tmp & 0x80);
    PSW_EQ (PSW_Z, !tmp);
}

// DEC - decrement destination
//
// Operation:
//  (dst) <- (dst) - 1
//
// Condition Codes:
//  N: set if result is <0; cleared otherwise
//  Z: set if result is 0; cleared otherwise
//  V: set if (dst) was 100000; cleared otherwise
//  C: not affected
//
// Subtract 1 from the contents of the destination
//
void KD11CPU::DEC (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (reg);
    CondData<u16> contents = operandLocation.wordContents ();
    if (!contents.hasValue ())
        return;

    // Increment the operand and write it to the operand location
    u16 result = contents - 1;
    operandLocation.write (result);

    PSW_EQ (PSW_V, contents == 0100000)
    PSW_EQ (PSW_N, result & 0100000);
    PSW_EQ (PSW_Z, !result);
}

// DECB - decrement destination byte
//
// Operation:
//  refer to DEC
// 
// Condition Codes:
//  refer to DEC
//
void KD11CPU::DECB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction decbInstruction {cpu, instruction};

    OperandLocation operandLocation = 
        decbInstruction.getOperandLocation (reg);
    CondData<u8> src = operandLocation.byteContents ();
    if (!src.hasValue ())
        return;

    u8 tmp = (u8) (src - 1);

    operandLocation.writeByte (tmp);

    PSW_EQ (PSW_V, src == 0000200)
        PSW_EQ (PSW_N, tmp & 0x80);
    PSW_EQ (PSW_Z, !tmp);
}

// NEG - negate destination
//
// Operation:
//  (dst) <- -(dst)
//
// Condition Codes:
//  N: set if the result is < 0; cleared otherwise
//  Z: set if result is O: cleared otherwise
//  V: set if the result is 100000; cleared otherwise
//  C: cleared if the result is O; set otherwise
//
// Replaces the contents of the destination address by its two's complement.
// Note that 100000 is replaced by itself (in two's complement notation the
// most negative number has no positive counterpart).
//
void KD11CPU::NEG (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (reg);
    CondData<u16> operand = operandLocation.wordContents ();
    if (!operand.hasValue ())
        return;

    // Negate the operand and write it to the operand location
    if (operand != 0100000)
        operand = -operand;
    
    operandLocation.write (operand);

    PSW_EQ (PSW_V, operand == 0100000)
    PSW_EQ (PSW_N, operand & 0100000);
    PSW_EQ (PSW_Z, !operand);
    PSW_EQ (PSW_C, operand);
}

// NEGB - negate destination byte
//
// Operation:
//  refer to NEG
// 
// Condition Codes:
//  refer to NEG
//
void KD11CPU::NEGB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction negbInstruction {cpu, instruction};

    OperandLocation operandLocation = 
        negbInstruction.getOperandLocation (reg);
    CondData<u8> operand = operandLocation.byteContents ();
    if (!operand.hasValue ())
        return;

    if (operand != 0200)
        operand = -operand;

    operandLocation.writeByte (operand);

    PSW_EQ (PSW_V, operand == 0200)
        PSW_EQ (PSW_N, operand & 0x80);
    PSW_EQ (PSW_Z, !operand);
    PSW_EQ (PSW_C, operand);
}

// ADC - add carry
//
// Operation:
//  (dst) <- (dst) + (C bit)
//
// Condition Codes:
//  N: set if result <0; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: set if (dst) was 077777 and (C) was l; cleared otherwise
//  C: set if (dst) was 177777 and (C) was l; cleared otherwise
//
// Adds the contents of the C-bit into the destination. This permits the
// carry from the addition of the low-order words to be carried into the
// high-order result.
//
void KD11CPU::ADC (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (reg);
    CondData<u16> contents = operandLocation.wordContents ();
    if (!contents.hasValue ())
        return;

    u16 cBit = PSW_GET (PSW_C) ? 1 : 0;
    u16 result = contents + cBit;

    operandLocation.write (result);

    PSW_EQ (PSW_V, contents == 0077777 && PSW_GET (PSW_C));
    PSW_EQ (PSW_C, contents == 0177777 && PSW_GET (PSW_C));
    PSW_EQ (PSW_N, result & 0x8000);
    PSW_EQ (PSW_Z, !result);
}

// SBC - subtract carry
//
// Operation
//  (dst) <- (dst) - (C)
//
// Condition Codes:
//  N: set if result <O; cleared otherwise
//  Z: set if result O; cleared otherwise
//  V: set if (dst) was 100000; cleared otherwise
//  C: set if (dst) was 0 and C was 1; cleared otherwise
//
// Subtracts the contents of the C-bit from the destination. This permits the
// carry from the subtraction of two low-order words to be subtracted from the
// high order part of the result.
void KD11CPU::SBC (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (reg);
    CondData<u16> contents = operandLocation.wordContents ();
    if (!contents.hasValue ())
        return;

    u16 cBit = PSW_GET (PSW_C) ? 1 : 0;
    u16 result = contents - cBit;

    operandLocation.write (result);

    PSW_EQ (PSW_V, contents == 0100000);
    PSW_EQ (PSW_C, !contents && PSW_GET (PSW_C));
    PSW_EQ (PSW_N, result & 0x8000);
    PSW_EQ (PSW_Z, !result);
}

// TST - test destination
//
// Operation
// (dst) <- (dst)
//
// Condition Codes:
//  N: set if the result is < 0; cleared otherwise
//  Z: set if result is 0; cleared otherwise
//  V: cleared
//  C: cleared
void KD11CPU::TST (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (reg);
    CondData<u16> contents = operandLocation.wordContents ();
    if (!contents.hasValue ())
        return;

    PSW_CLR (PSW_V);
    PSW_CLR (PSW_C);
    PSW_EQ (PSW_N, contents & 0100000);
    PSW_EQ (PSW_Z, !contents);
}

// ROR - rotate right
//
// Operation:
//  (dst) <- (dst) rotated right one place
//
// Condition Codes:
//  N: set if the high-order bit of the result is set (result < 0);
//     cleared otherwise
//  Z: set if all bits of result - 0; cleared otherwise
//  V: loaded with the Exclusive OR of the N-bit and C-bi.t (as set by the
//     completion of the rotate operation)
//  C: loaded with the low-order bit of the destination
//
// Rotates all bits of the destination right one place. Bit 0 is loaded into
// the C-bit and the previous contents of the C-bit are loaded into bit 15 of
// the destination.
//
void KD11CPU::ROR (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (reg);
    CondData<u16> contents = operandLocation.wordContents ();
    if (!contents.hasValue ())
        return;

    u16 cBit = PSW_GET (PSW_C);
    u16 result = contents >> 1;
    if (cBit)
        result |= 0100000;

    operandLocation.write (result);

    PSW_EQ (PSW_C, contents & 0000001);
    PSW_EQ (PSW_N, result & 0100000);
    PSW_EQ (PSW_Z, !result);
    PSW_EQ (PSW_V, PSW_GET (PSW_N) ^ PSW_GET (PSW_C));
}

// ROL - rotate left
// 
// Operation:
//  (dst) <- (dst) rotated left one place
//
// Condition Codes:
//  N: set if the high-order bit of the result word is set (result < 0);
//     cleared otherwise
//  Z: set if all bits of the result word = O; cleared otherwise
//  V: loaded with the Exclusive OR of the N-bit and C-bit (as set by the
//     completion of the rotate operation)
//  C: loaded with the high-order bit of the destination
//
// Rotate all bits of the destination left one place. Bit 15 is loaded into
// the C-bit of the status word and the previous contents of the C-bit are
// loaded into Bit 0 of the destination.
//
void KD11CPU::ROL (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (reg);
    CondData<u16> contents = operandLocation.wordContents ();
    if (!contents.hasValue ())
        return;

    u16 cBit = PSW_GET (PSW_C);
    u16 result = contents << 1;
    if (cBit)
        result |= 01;

    operandLocation.write (result);

    PSW_EQ (PSW_C, contents & 0100000);
    PSW_EQ (PSW_N, result & 0100000);
    PSW_EQ (PSW_Z, !result);
    PSW_EQ (PSW_V, PSW_GET (PSW_N) ^ PSW_GET (PSW_C));
}

// ASR - arithmetic shift right
// 
// Operation:
//  (dst) <- (dst) shifted one place to the right
//
// Condition Codes:
//  N: set if the high-order bit of the result is set (result < 0);
//     cleared otherwise
//  Z: set if the result - O; cleared otherwise
//  V: loaded from the Exclusive OR of the N-bit and C-.bit (as set by the
//     completion of the shift operation)
//  C: loaded from low-order bit of the destination
//
// Shifts all bits of the destination right one place. Bit 15 is reproduced.
// The C-bit is loaded from bit 0 of the destination. ASR performs signed
// division of the destination by two.
//
void KD11CPU::ASR (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (reg);
    CondData<u16> contents = operandLocation.wordContents ();
    if (!contents.hasValue ())
        return;;

    u16 result = contents;
    if (result & 0100000)
    {
        result >>= 1;
        result |= 0100000;
    }
    else
        result >>= 1;

    operandLocation.write (result);

    PSW_EQ (PSW_C, contents & 1);
    PSW_EQ (PSW_N, result & 0100000);
    PSW_EQ (PSW_Z, !result);
    PSW_EQ (PSW_V, PSW_GET (PSW_N) ^ PSW_GET (PSW_C));
}

// ASL - arithmetic shift left
//
// Operation:
//  (dst) <- (dst) shifted one place to the left
//
// Condition Codes:
//  N: set if high-order bit of the result is set (result < 0); cleared
//     otherwise
//  Z: set if the result = O; cleared otherwise
//  V: loaded with the exclusive OR of the N-bit and C-bit (as set by the
//     completion of the shift operation)
//  C: loaded with the high-order bit of the destination
//
// Shifts all bits of the destination left one place. Bit 0 is loaded with
// an 0. The C-bit of the status word is loaded from the most significant bit
// of the destination. ASL performs a signed multiplication of the destination
// by 2 with overflow indication.
//
void KD11CPU::ASL (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (reg);
    CondData<u16> contents = operandLocation.wordContents ();
    if (!contents.hasValue ())
        return;

    u16 result = contents << 1;

    operandLocation.write (result);

    PSW_EQ (PSW_C, contents & 0100000);
    PSW_EQ (PSW_N, result & 0100000);
    PSW_EQ (PSW_Z, !result);
    PSW_EQ (PSW_V, PSW_GET (PSW_N) ^ PSW_GET (PSW_C));
}

// SXT - sign extend
//
// Operation:
//  (dst) <- 0 if N bit is clear
//  (dst) <- 1 if N bit is set
//
// Condition Codes:
//  N: unaffected
//  Z: set if N bit clear
//  V: cleared
//  C: unaffected
//
// If the condition code bit N is set then a -1 is placed in the destination
// operand: if N bit is clear, then a 0 is placed in the destination operand.
//
void KD11CPU::SXT (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (reg);
    u16 result = PSW_GET (PSW_N) ? 0177777 : 0;

    operandLocation.write (result);

    PSW_EQ (PSW_Z, !PSW_GET (PSW_N));
    PSW_CLR (PSW_V);
}