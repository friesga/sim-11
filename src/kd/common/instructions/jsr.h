#ifndef _JSR_H_
#define _JSR_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/eisinstruction/eisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

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
class CommonInstruction::JSR : public EisInstruction, public WithFactory<JSR>
{
public:
    JSR (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::JSR::JSR (CpuData* cpu, u16 instruction)
    :
    EisInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::JSR::execute ()
{
    OperandLocation destination = getOperandLocation (cpu_->registers ());

    if (!destination.isA<MemoryOperandLocation> ())
    {
        // Illegal instruction
        trace.trap (TrapRecordType::TRAP_RADDR, 04);
        return CpuData::Trap::BusError;
    }

    GeneralRegisters& registers = cpu_->registers ();
    u16 specifiedRegisterContents = registers[getRegisterNr ()];

    cpu_->pushWord (specifiedRegisterContents);
    registers[getRegisterNr ()] = registers[7];
    registers[7] = destination;

    return CpuData::Trap::None;
}

#endif // _JSR_H_