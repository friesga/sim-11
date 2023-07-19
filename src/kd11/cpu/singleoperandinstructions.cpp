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