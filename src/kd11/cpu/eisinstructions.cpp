#include "eisinstruction/eisinstruction.h"
#include "trace/trace.h"

#include "bitmask.h"
#include "kd11insnjsr/kd11insnjsr.h"

// JSR - jump to subroutine
//
// Operation:
//  v(SP) <- reg (push reg contents onto processor stack)
//  reg <- PC    (PC holds location following JSR; this address now put in reg)
//  PC <- (dst)  (PC now points to subroutine destination)
//
// In execution of the JSR, the old contents of the specified register (the
// "LINKAGE POINTER") are automatically pushed onto the processor stack and
// new linkage information placed in the register.
//
// A subroutine called with a JSR reg,dst instruction can access the arguments
// following the call with either autoincrement addressing, (reg)+, (if
// arguments are accessed sequentially) or by indexed addressing, X(reg),
// (if accessed in random order). These addressing modes may also be deferred,
// @(reg) + and @X(reg) if the parameters are operand addresses rather than
// the operands themselves.
// 
// JMP and JSR with register mode destinations are illegal instructions, and
// trap to vector address 4.
//
void KD11CPU::JSR (KD11CPU* cpu, u16 (&registers)[8], u16 instruction)
{
    EisInstruction jsrInstruction {cpu, instruction};
    OperandLocation destination = jsrInstruction.getOperandLocation (registers);

    if (!destination.isA<CondData<u16>> ())
    {
        // Illegal instruction
        trace.trap (TrapRecordType::TRAP_RADDR, 04);
        setTrap (&busError);
        return;
    }

    u16 reg = register_[jsrInstruction.getRegisterNr ()];

    register_[6] -= 2;
    putWord (register_[6], reg);
    register_[jsrInstruction.getRegisterNr ()] = register_[7];
    register_[7] = destination;
}
// MUL - multiply
//
// Operation:
//  R, R+1 <- R x (src)
//
//  Condition Codes:
//  N: set if product is <0: cleared otherwise
//  Z: set if product is O: cleared otherwise
//  V: cleared
//  C: set if the result is less than-215 or greater than or equal to 2^15 - 1
//
// The contents of the destination register and source taken as two's complement
// integers are multiplied and stored in the destination register and the
// succeeding register (if R is even). If R is odd only the low order product
// is stored.
//
void KD11CPU::MUL (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    EisInstruction mulInstruction (cpu, instruction);
    u16 regNr = mulInstruction.getRegisterNr ();

    u16 dst = register_[regNr];

    OperandLocation sourceOperandLocation = 
        mulInstruction.getOperandLocation (reg);
    CondData<u16> source = sourceOperandLocation.wordContents ();
    
    s32 tmps32 = (s32)(s16)dst * (s16) source;
    register_[regNr] = (u16) (tmps32 >> 16);
    register_[regNr | 1] = (u16) tmps32;

    PSW_CLR (PSW_V);
    PSW_EQ (PSW_N, tmps32 < 0);
    PSW_EQ (PSW_Z, !tmps32);
    PSW_EQ (PSW_C, (tmps32 >= 0x7FFF) || (tmps32 < -0x8000));
}

// DIV - divide
//
// Operation:
//  R,R+1 <- R,R+1 / (src)
//
// Condition Codes:
//  N: set if quotient <0: cleared otherwise
//  Z: set if quotient = 0: cleared otherwise
//  V: set if source = 0 or if the absolute value of the register is larger
//     than the absolute value of the source. (In this case the instruction
//     is aborted because the quotient would exceed 15 bits.)
//  C: set if divide 0 attempted; cleared otherwise
//
// The 32-bit two's complement integer in R and R+1 is divided by the source
// operand. The quotient is left in R: the remainder in R+1. Division will be
// performed so that the remainder is of the same sign as the dividend. R must
// be even.
//
void KD11CPU::DIV (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    EisInstruction divInstruction (cpu, instruction);
    u16 regNr = divInstruction.getRegisterNr ();

    s32 tmps32 = (register_[regNr] << 16) | register_[regNr| 1];

    OperandLocation sourceOperandLocation = 
        divInstruction.getOperandLocation (reg);
    CondData<u16> source = sourceOperandLocation.wordContents ();
    if (!source.hasValue ())
        return;

    if (source == 0)
    {
        PSW_SET (PSW_C);
        PSW_SET (PSW_V);
    }
    else
    {
        s32 quotient = tmps32 / (s16) source;
        s32 remainder = tmps32 % (s16) source;
        PSW_CLR (PSW_C);

        if ((s16) quotient != quotient)
        {
            PSW_SET (PSW_V);
        }
        else
        {
            register_[regNr] = (u16) quotient;
            register_[regNr | 1] = (u16) remainder;
            PSW_EQ (PSW_Z, !quotient);
            PSW_EQ (PSW_N, quotient < 0);
        }
    }
}

// ASH - shift arithmetically
//
// Operation:
//  R <- R shifted arithmetically NN places to right or left where
//  NN = low order 6 bits of source
//
// Condition Codes:
//  N: set if result <O; cleared otherwise
//  Z: set if result = 0; cleared otherwise
//  V: set if sign of register changed during shift; cleared otherwise
//  C: loaded from last bit shifted out of register
//
// The contents of the register are shifted right or left the number of times
// specified by the shift count. The shift count is taken as the low order
// 6 bits of the source operand. This number ranges from -32 to + 31. Negative
// is a a right shift and positive is a left shift.
//
void KD11CPU::ASH (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    EisInstruction ashInstruction (cpu, instruction);
    u16 regNr = ashInstruction.getRegisterNr ();
    u16 tmp {0};
    u16 dst = register_[regNr];

    OperandLocation sourceOperandLocation = 
        ashInstruction.getOperandLocation (reg);
    CondData<u16> source = sourceOperandLocation.wordContents ();
    if (!source.hasValue ())
        return;

    if (source & 0x20)
    { 
        // Negative - shift right
        s16 stmp, stmp2;
        source = (~source & 0x1F) + 1;
        stmp = (s16) dst;
        stmp2 = stmp >> (source - 1);
        stmp >>= source;
        tmp = (u16) stmp;
        PSW_EQ (PSW_C, stmp2 & 1);
        PSW_CLR (PSW_V);
    }
    else if ((source & 0x1F) == 0)
    {
        // Zero - don't shift
        PSW_CLR (PSW_V);
        PSW_CLR (PSW_C);
        tmp = dst;
    }
    else
    {
        // Positive - shift left
        s16 mask = 0;
        // Note that the bitwise and/assignment operator can't be used on
        // CondData objects.
        source = source & 0x1F;
        tmp = dst << source;
        if (source > 0)
        {
            mask = 0x8000;
            mask >>= source;
            u16 tmp2 = dst & mask;
            PSW_EQ (PSW_V, !((tmp2 == 0) || (((tmp2 & mask) | ~mask) == 0xFFFF)));
        }
        else
        {
            PSW_CLR (PSW_V);
        }
        PSW_EQ (PSW_C, (dst << (source - 1)) & 0x8000);
        if ((dst & 0x8000) != (tmp & 0x8000))
        {
            PSW_SET (PSW_V);
        }
    }
    register_[regNr] = tmp;
    PSW_EQ (PSW_N, tmp & 0x8000);
    PSW_EQ (PSW_Z, !tmp);
}

// ASHC - arithemetic shift combined
//
// Operation:
//  R, R+1 <- R, R+1 The double word is shifted NN places to the right or
//  left, where NN =low order six bits of source
//
// Condition Codes:
//  N: set if result <0; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: set if sign bit changes during the shift; cleared otherwise
//  C: loaded with high order bit when left shift; loaded with low order bit
//     when right shift (loaded with the last bit shifted out of the 32-bit
//     operand)
//
// The contents of the register and the register ORed with one are treated as
// one 32 bit word, R + 1 (bits 0-15) and R (bits 16-31) are shifted right or
// left the number of times specified by the shift count. The shift count is
// taken as the low order 6 bits of the source operand. This number ranges
// from -32 to +31. Negative is a right shift and positive is a left shift.
// 
// When the register chosen is an odd number the register and the register
// OR'ed with one are the same. In this case the right shift becomes a rotate
// (for up to a shift of 16). The 16 bit word is rotated right the number of
// bits specified by the shift count.
//
void KD11CPU::ASHC (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    EisInstruction ashcInstruction (cpu, instruction);
    u16 regNr = ashcInstruction.getRegisterNr ();
    u16 tmp {0};
    u16 dst = register_[regNr];

    s32 tmps32 = (register_[regNr] << 16) | register_[regNr | 1];

    OperandLocation sourceOperandLocation = 
        ashcInstruction.getOperandLocation (reg);
    CondData<u16> source = sourceOperandLocation.wordContents ();
    if (!source.hasValue ())
        return;

    if ((source & 0x3F) == 0x20)
    {
        // Negative; 32 right
        PSW_EQ (PSW_C, tmps32 & 0x80000000);
        PSW_CLR (PSW_V);
        if (PSW_GET (PSW_C))
        {
            tmps32 = 0xFFFFFFFF;
        }
        else
        {
            tmps32 = 0;
        }
    }
    else if (source & 0x20)
    {
        // Negative - shift right
        s32 stmp2;
        source = (~source & 0x1F) + 1;
        stmp2 = tmps32 >> (source - 1);
        tmps32 >>= source;
        PSW_EQ (PSW_C, stmp2 & 1);
    }
    else if ((source & 0x1F) == 0)
    {
        // Zero - don't shift
        PSW_CLR (PSW_V);
        PSW_CLR (PSW_C);
    }
    else
    {
        // Positive - shift left
        s32 stmp2;
        source = source & 0x1F;
        stmp2 = tmps32 << (source - 1);
        tmps32 <<= source;
        PSW_EQ (PSW_C, stmp2 & 0x80000000);
        PSW_EQ (PSW_V, !!(dst & 0x8000)
            != !!(tmps32 & 0x80000000));
    }
    register_[regNr] = (u16)(tmps32 >> 16);
    register_[regNr | 1] = (u16)tmps32;
    PSW_EQ (PSW_N, tmps32 & 0x80000000);
    PSW_EQ (PSW_Z, !tmps32);
}

// XOR - exclusive OR
//
// Operation:
//  (dst) <- R v (dst)
//
// Condition Codes:
//  N: set if the result <0: cleared otherwise
//  Z: set if result = O: cleared otherwise
//  V: cleared
//  C: unaffected
//
// The exclusive OR of the register and destination operand is stored in the
// destination address. Contents of register are unaffected.
//
void KD11CPU::XOR (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    EisInstruction ashcInstruction (cpu, instruction);
    u16 regNr = ashcInstruction.getRegisterNr ();

    u16 source = register_[regNr];

    OperandLocation destinationOperandLocation = 
        ashcInstruction.getOperandLocation (reg);
    CondData<u16> destination = destinationOperandLocation.wordContents ();
    if (!destination.hasValue ())
        return;

    u16 result = source ^ destination;

    destinationOperandLocation.write (result);

    PSW_EQ (PSW_N, result & 0x8000);
    PSW_EQ (PSW_Z, !result);
    PSW_CLR (PSW_V);
}