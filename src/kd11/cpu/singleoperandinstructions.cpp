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
    if (!operandLocation.isValid ())
        return;

    u16 operand = operandLocation.contents ();

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
    if (!operandLocation.isValid ())
        return;

    operandLocation.write (0);
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
    if (!operandLocation.isValid ())
        return;

    u16 operand = operandLocation.contents ();

    // Complement the operand and write it to the operand location
    operand = ~operand;
    operandLocation.write (operand);

    PSW_CLR (PSW_V);
    PSW_SET (PSW_C);
    PSW_EQ (PSW_N, operand & 0x8000);
    PSW_EQ (PSW_Z, !operand);
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
    if (!operandLocation.isValid ())
        return;

    u16 contents = operandLocation.contents ();

    // Increment the operand and write it to the operand location
    u16 result = contents + 1;
    operandLocation.write (result);

    PSW_EQ (PSW_V, contents == 077777)
    PSW_EQ (PSW_N, result & 0x8000);
    PSW_EQ (PSW_Z, !result);
}