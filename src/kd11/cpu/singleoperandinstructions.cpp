#include "kd11cpu.h"
#include "trace/trace.h"
#include "singleoperandinstruction/singleoperandinstruction.h"

/*
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
void KD11CPU::JMP (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};
    OperandLocation location = soi.getOperandLocation (cpu->registers ());

    if (location.isA<CondData<u16>> ())
        cpu->registers ()[7] = location;
    else
    {
        // Illegal instruction
        trace.trap (TrapRecordType::TRAP_RADDR, 04);
        setTrap (&busError);
    }
}
*/

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
/*
void KD11CPU::SWAB (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (cpu->registers ());
    CondData<u16> operand = operandLocation.wordContents ();
    if (!operand.hasValue ())
        return;

    // Swap bytes in the operand and write it to the operand location
    operand = ((operand & 0x00FF) << 8) | ((operand >> 8) & 0xFF);
    operandLocation.write (operand);

    setConditionCodeIf_ClearElse (PSW_N, operand & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !((u8) operand));
    clearConditionCode (PSW_V);
    clearConditionCode (PSW_C);
}
*/
/*
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
void KD11CPU::CLR (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (cpu->registers ());
    operandLocation.write (0);

    clearConditionCode (PSW_N | PSW_V | PSW_C);
    setConditionCode (PSW_Z);
}
*/

// CLRB - clear destination byte
//
// Operation:
//  refer to CLR
// 
// Condition Codes:
//  refer to CLR
//
void KD11CPU::CLRB (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction clrbInstruction {cpu, instruction};
    OperandLocation operandLocation =
        clrbInstruction.getOperandLocation (cpu->registers ());

    operandLocation.writeByte (0);

    clearConditionCode (PSW_N | PSW_V | PSW_C);
    setConditionCode (PSW_Z);
}

/*
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
void KD11CPU::COM (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (cpu->registers ());
    CondData<u16> operand = operandLocation.wordContents ();
    if (!operand.hasValue ())
        return;

    // Complement the operand and write it to the operand location
    operand = ~operand;
    operandLocation.write (operand);

    clearConditionCode (PSW_V);
    setConditionCode (PSW_C);
    setConditionCodeIf_ClearElse (PSW_N, operand & 0x8000);
    setConditionCodeIf_ClearElse (PSW_Z, !operand);
}
*/

// COMB - complement destination byte
//
// Operation:
//  refer to COM
// 
// Condition Codes:
//  refer to COM
//
void KD11CPU::COMB (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction combInstruction {cpu, instruction};

    OperandLocation operandLocation = 
        combInstruction.getOperandLocation (cpu->registers ());
    CondData<u8> operand = operandLocation.byteContents ();
    if (!operand.hasValue ())
        return;

    // Complement the operand and write it to the operand location
    operand = ~operand;
    operandLocation.writeByte (operand);

    clearConditionCode (PSW_V);
    setConditionCode (PSW_C);
    setConditionCodeIf_ClearElse (PSW_N, operand & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !((u8) operand));
}

/*
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
void KD11CPU::INC (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (cpu->registers ());
    CondData<u16> contents = operandLocation.wordContents ();
    if (!contents.hasValue ())
        return;

    // Increment the operand and write it to the operand location
    u16 result = contents + 1;
    operandLocation.write (result);

    setConditionCodeIf_ClearElse (PSW_V, contents == 077777);
    setConditionCodeIf_ClearElse (PSW_N, result & 0x8000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
}
*/

// INCB - increment destination byte
//
// Operation:
//  refer to INC
// 
// Condition Codes:
//  refer to INC
//
void KD11CPU::INCB (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction incbInstruction {cpu, instruction};

    OperandLocation operandLocation = 
        incbInstruction.getOperandLocation (cpu->registers ());
    CondData<u8> src = operandLocation.byteContents ();
    if (!src.hasValue ())
        return;

    u8 tmp = (u8) (src + 1);

    operandLocation.writeByte (tmp);

    setConditionCodeIf_ClearElse (PSW_V, src == 000177);
    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
}

/*
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
void KD11CPU::DEC (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (cpu->registers ());
    CondData<u16> contents = operandLocation.wordContents ();
    if (!contents.hasValue ())
        return;

    // Increment the operand and write it to the operand location
    u16 result = contents - 1;
    operandLocation.write (result);

    setConditionCodeIf_ClearElse (PSW_V, contents == 0100000);
    setConditionCodeIf_ClearElse (PSW_N, result & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
}
*/

// DECB - decrement destination byte
//
// Operation:
//  refer to DEC
// 
// Condition Codes:
//  refer to DEC
//
void KD11CPU::DECB (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction decbInstruction {cpu, instruction};

    OperandLocation operandLocation = 
        decbInstruction.getOperandLocation (cpu->registers ());
    CondData<u8> src = operandLocation.byteContents ();
    if (!src.hasValue ())
        return;

    u8 tmp = (u8) (src - 1);

    operandLocation.writeByte (tmp);

    setConditionCodeIf_ClearElse (PSW_V, src == 0000200);
    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
}

/*
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
void KD11CPU::NEG (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (cpu->registers ());
    CondData<u16> operand = operandLocation.wordContents ();
    if (!operand.hasValue ())
        return;

    // Negate the operand and write it to the operand location
    if (operand != 0100000)
        operand = -operand;
    
    operandLocation.write (operand);

    setConditionCodeIf_ClearElse (PSW_V, operand == 0100000);
    setConditionCodeIf_ClearElse (PSW_N, operand & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !operand);
    setConditionCodeIf_ClearElse (PSW_C, operand);
}
*/

// NEGB - negate destination byte
//
// Operation:
//  refer to NEG
// 
// Condition Codes:
//  refer to NEG
//
void KD11CPU::NEGB (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction negbInstruction {cpu, instruction};

    OperandLocation operandLocation = 
        negbInstruction.getOperandLocation (cpu->registers ());
    CondData<u8> operand = operandLocation.byteContents ();
    if (!operand.hasValue ())
        return;

    if (operand != 0200)
        operand = -operand;

    operandLocation.writeByte (operand);

    setConditionCodeIf_ClearElse (PSW_V, operand == 0200);
    setConditionCodeIf_ClearElse (PSW_N, operand & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !operand);
    setConditionCodeIf_ClearElse (PSW_C, operand);
}

/*/
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
void KD11CPU::ADC (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (cpu->registers ());
    CondData<u16> contents = operandLocation.wordContents ();
    if (!contents.hasValue ())
        return;

    u16 cBit = isSet (PSW_C) ? 1 : 0;
    u16 result = contents + cBit;

    operandLocation.write (result);

    setConditionCodeIf_ClearElse (PSW_V, contents == 0077777 && isSet (PSW_C));
    setConditionCodeIf_ClearElse (PSW_C, contents == 0177777 && isSet (PSW_C));
    setConditionCodeIf_ClearElse (PSW_N, result & 0x8000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
}
*/

// ADCB - add carry byte
//
// Operation:
//  refer to ADC
// 
// Condition Codes:
//  refer to ADC
//
void KD11CPU::ADCB (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction adcbInstruction {cpu, instruction};

    OperandLocation operandLocation = 
        adcbInstruction.getOperandLocation (cpu->registers ());
    CondData<u8> source = operandLocation.byteContents ();
    if (!source.hasValue ())
        return;

    u16 tmp = isSet (PSW_C) ? 1 : 0;
    u16 destination = (u8) (source + tmp);

    operandLocation.writeByte (destination);

    setConditionCodeIf_ClearElse (PSW_V, source == 0177 && isSet (PSW_C));
    setConditionCodeIf_ClearElse (PSW_C, source == 0377 && isSet (PSW_C));
    setConditionCodeIf_ClearElse (PSW_N, destination & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !destination);
}

/*
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
void KD11CPU::SBC (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (cpu->registers ());
    CondData<u16> contents = operandLocation.wordContents ();
    if (!contents.hasValue ())
        return;

    u16 cBit = isSet (PSW_C) ? 1 : 0;
    u16 result = contents - cBit;

    operandLocation.write (result);

    setConditionCodeIf_ClearElse (PSW_V, contents == 0100000);
    setConditionCodeIf_ClearElse (PSW_C, !contents && isSet (PSW_C));
    setConditionCodeIf_ClearElse (PSW_N, result & 0x8000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
}
*/

// SBCB - subtract carry byte
//
// Operation:
//  refer to SBC
// 
// Condition Codes:
//  refer to SBC
//
void KD11CPU::SBCB (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction sbcbInstruction {cpu, instruction};

    OperandLocation operandLocation = 
        sbcbInstruction.getOperandLocation (cpu->registers ());
    CondData<u8> source = operandLocation.byteContents ();
    if (!source.hasValue ())
        return;

    u16 tmp = isSet (PSW_C) ? 1 : 0;
    u16 destination = (u8) (source - tmp);

    operandLocation.writeByte (destination);

    setConditionCodeIf_ClearElse (PSW_V, source == 0200);
    setConditionCodeIf_ClearElse (PSW_C, !source && isSet (PSW_C));
    setConditionCodeIf_ClearElse (PSW_N, destination & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !destination);
}

/*
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
void KD11CPU::TST (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (cpu->registers ());
    CondData<u16> contents = operandLocation.wordContents ();
    if (!contents.hasValue ())
        return;

    clearConditionCode (PSW_V);
    clearConditionCode (PSW_C);
    setConditionCodeIf_ClearElse (PSW_N, contents & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !contents);
}
*/

// TSTB - test destination byte
//
// Operation:
//  refer to TST
// 
// Condition Codes:
//  refer to TST
//
void KD11CPU::TSTB (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction tstbInstruction {cpu, instruction};

    OperandLocation operandLocation = 
        tstbInstruction.getOperandLocation (cpu->registers ());
    CondData<u8> source = operandLocation.byteContents ();
    if (!source.hasValue ())
        return;

    clearConditionCode (PSW_V);
    clearConditionCode (PSW_C);
    setConditionCodeIf_ClearElse (PSW_N, source & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !source);
}

/*
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
void KD11CPU::ROR (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (cpu->registers ());
    CondData<u16> contents = operandLocation.wordContents ();
    if (!contents.hasValue ())
        return;

    u16 cBit = isSet (PSW_C);
    u16 result = contents >> 1;
    if (cBit)
        result |= 0100000;

    operandLocation.write (result);

    setConditionCodeIf_ClearElse (PSW_C, contents & 0000001);
    setConditionCodeIf_ClearElse (PSW_N, result & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    setConditionCodeIf_ClearElse (PSW_V, isSet (PSW_N) ^ isSet (PSW_C));
}
*/

// RORB - rotate right byte
//
// Operation:
//  refer to ROR
// 
// Condition Codes:
//  refer to ROR
//
// Same as ROR instruction with the distintion that for odd adresses the
// carry bit is loaded in bit 15 of the word and for even addresses the carry
// bit is loaded in bit 7 of the word.
//
void KD11CPU::RORB (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction rorbInstruction {cpu, instruction};

    OperandLocation operandLocation = 
        rorbInstruction.getOperandLocation (cpu->registers ());
    CondData<u8> source = operandLocation.byteContents ();
    if (!source.hasValue ())
        return;

    u16 result = source >> 1;
    if (isSet (PSW_C))
        result |= 0x80;

    operandLocation.writeByte (result);

    setConditionCodeIf_ClearElse (PSW_C, source & 0x01);
    setConditionCodeIf_ClearElse (PSW_N, result & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    setConditionCodeIf_ClearElse (PSW_V, isSet (PSW_N) ^ isSet (PSW_C));
}

/*
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
void KD11CPU::ROL (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (cpu->registers ());
    CondData<u16> contents = operandLocation.wordContents ();
    if (!contents.hasValue ())
        return;

    u16 cBit = isSet (PSW_C);
    u16 result = contents << 1;
    if (cBit)
        result |= 01;

    operandLocation.write (result);

    setConditionCodeIf_ClearElse (PSW_C, contents & 0100000);
    setConditionCodeIf_ClearElse (PSW_N, result & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    setConditionCodeIf_ClearElse (PSW_V, isSet (PSW_N) ^ isSet (PSW_C));
}
*/

// ROLB - rotate left byte
//
// Operation:
//  refer to ROL
// 
// Condition Codes:
//  refer to ROL
//
// Same as ROL instruction with the distinction that for odd adresses the
// carry bit is loaded with bit 15 of the word and for even addresses the carry
// bit is loaded with bit 7 of the word.
//
void KD11CPU::ROLB (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction rolbInstruction {cpu, instruction};

    OperandLocation operandLocation = 
        rolbInstruction.getOperandLocation (cpu->registers ());
    CondData<u8> source = operandLocation.byteContents ();
    if (!source.hasValue ())
        return;

    u16 result = (u8)(source << 1);
    if (isSet (PSW_C))
        result |= 0x01;

    operandLocation.writeByte (result);

    setConditionCodeIf_ClearElse (PSW_C, source & 0x80);
    setConditionCodeIf_ClearElse (PSW_N, result & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    setConditionCodeIf_ClearElse (PSW_V, isSet (PSW_N) ^ isSet (PSW_C));
}

/*
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
void KD11CPU::ASR (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (cpu->registers ());
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

    setConditionCodeIf_ClearElse (PSW_C, contents & 1);
    setConditionCodeIf_ClearElse (PSW_N, result & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    setConditionCodeIf_ClearElse (PSW_V, isSet (PSW_N) ^ isSet (PSW_C));
}
*/

// ASRB - arithmetic shift right byte
//
// Operation:
//  refer to ASR
// 
// Condition Codes:
//  refer to ASR
//
// Same as ASR instruction with the distinction that for odd adresses bit 15
// is reproduced and for even addresses bit 7 is reproduced.
//
void KD11CPU::ASRB (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction asrbInstruction {cpu, instruction};

    OperandLocation operandLocation = 
        asrbInstruction.getOperandLocation (cpu->registers ());
    CondData<u8> source = operandLocation.byteContents ();
    if (!source.hasValue ())
        return;

    u16 result = source;
    if (result & 0x80)
    {
        result >>= 1;
        result |= 0x80;
    }
    else
        result >>= 1;

    operandLocation.writeByte (result);

    setConditionCodeIf_ClearElse (PSW_C, source & 1);
    setConditionCodeIf_ClearElse (PSW_N, result & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    setConditionCodeIf_ClearElse (PSW_V, isSet (PSW_N) ^ isSet (PSW_C));
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
void KD11CPU::ASL (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (cpu->registers ());
    CondData<u16> contents = operandLocation.wordContents ();
    if (!contents.hasValue ())
        return;

    u16 result = contents << 1;

    operandLocation.write (result);

    setConditionCodeIf_ClearElse (PSW_C, contents & 0100000);
    setConditionCodeIf_ClearElse (PSW_N, result & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    setConditionCodeIf_ClearElse (PSW_V, isSet (PSW_N) ^ isSet (PSW_C));
}

// ASLB - arithmetic shift left byte
//
// Operation:
//  refer to ASL
// 
// Condition Codes:
//  refer to ASL
//
// Same as ASL instruction with the distinction that for odd adresses the
// carry bit is loaded from bit 15 of the word and bit 8 is loaded with a zero
// and for even addresses the carry bit is loaded from bit 7 and bit 0 is
// loaded with a zero.
//
void KD11CPU::ASLB (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction aslbInstruction {cpu, instruction};

    OperandLocation operandLocation = 
        aslbInstruction.getOperandLocation (cpu->registers ());
    CondData<u8> source = operandLocation.byteContents ();
    if (!source.hasValue ())
        return;

    u16 result = (u8) (source << 1);

    operandLocation.writeByte (result);

    setConditionCodeIf_ClearElse (PSW_C, source & 0x80);
    setConditionCodeIf_ClearElse (PSW_N, result & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    setConditionCodeIf_ClearElse (PSW_V, isSet (PSW_N) ^ isSet (PSW_C));
}

// MTPS - Move byte to Processor Status Word
//
// Operation:
//  PSW <- (src)
//
// Condition Codes:
//  Set according to effective SRC operand bits 0-3
//
// The 8 bits of the effective operand replaces the current contents of the
// PSW. The source operand address is treated as a byte address. Note that the
// T bit (PSW bit 4) cannot be set with this instruction. The SRC operand
// remains unchanged. This instruction can be used to change the priority bit
// (PSW bit 7) in the PSW
//
void KD11CPU::MTPS (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction mtpsInstruction {cpu, instruction};

    OperandLocation operandLocation = 
        mtpsInstruction.getOperandLocation (cpu->registers ());
    CondData<u8> newValue = operandLocation.byteContents ();
    if (!newValue.hasValue ())
        return;

    // Allow bits 5/6/7 to be set and cleared
    psw_ = (psw_ & PSW_T) | (newValue & ~PSW_T);
}

// MFPS - Move byte from Processor Status Word
//
// Operation:
//  (dst) < PSW
//  dst lower 8 bits
//
// Condition Codes:
//  N = set if PSW bit 7 = 1; cleared otherwise
//  Z = set if PS <0:7> = O; cleared otherwise·
//  V = cleared
//  C = not affected
// 
// The 8 bit contents of the PS are moved to the effective destination. If
// destination is mode 0, PS bit 7 is sign extended through upper byte of the
// register. The destination operand address is treated as a byte address.
//
void KD11CPU::MFPS (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction mfpsInstruction {cpu, instruction};

    OperandLocation operandLocation = 
        mfpsInstruction.getOperandLocation (cpu->registers ());

    u16 contents = (u8) psw_;

    if (operandLocation.isA<u8> ())
    {
        // If destination is mode 0 (Register), the regular operand processing
        // is bypassed and PS bit 7 is sign extended through the upper byte of
        // the register.
        register_[operandLocation] = (s8) psw_;
    }
    else
    {
        if (!operandLocation.writeByte (contents))
            return;
    }
    setConditionCodeIf_ClearElse (PSW_N, contents & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !(contents & 0xFF));
    clearConditionCode (PSW_V);
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
void KD11CPU::SXT (CpuData* cpu, u16 instruction)
{
    SingleOperandInstruction soi {cpu, instruction};

    OperandLocation operandLocation = soi.getOperandLocation (cpu->registers ());
    u16 result = isSet (PSW_N) ? 0177777 : 0;

    operandLocation.write (result);

    setConditionCodeIf_ClearElse (PSW_Z, !isSet (PSW_N));
    clearConditionCode (PSW_V);
}