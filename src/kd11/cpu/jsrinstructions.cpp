#include "jsrinstruction/jsrinstruction.h"
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
void KD11CPU::JSR (KD11CPU* cpu, u16 (&registers)[8], u16 instruction)
{
    JsrInstruction jsrInstruction {cpu, instruction};
    OperandLocation destination = jsrInstruction.getOperandLocation (registers);

    if (!destination.isValid ())
    {
        // Illegal instruction
        trace.trap (TrapRecordType::TRAP_RADDR, 04);
        setTrap (&busError);
        return;
    }

    u16 reg = register_[jsrInstruction.getLinkageRegister ()];

    register_[6] -= 2;
    putWord (register_[6], reg);
    register_[jsrInstruction.getLinkageRegister ()] = register_[7];
    register_[7] = destination;
}