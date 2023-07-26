#include "eisinstruction/eisinstruction.h"
#include "trace/trace.h"

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
    CondData<u16> source = sourceOperandLocation.contents ();
    
    s32 tmps32 = (s32)(s16)dst * (s16) source;
    register_[regNr] = (u16) (tmps32 >> 16);
    register_[regNr | 1] = (u16) tmps32;

    PSW_CLR (PSW_V);
    PSW_EQ (PSW_N, tmps32 < 0);
    PSW_EQ (PSW_Z, !tmps32);
    PSW_EQ (PSW_C, (tmps32 >= 0x7FFF) || (tmps32 < -0x8000));
}