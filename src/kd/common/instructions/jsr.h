#ifndef _JSR_H_
#define _JSR_H_

#include "commoninstruction.h"
#include "kd/common/eisinstruction/eisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
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
    JSR (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::JSR::JSR (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    EisInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::JSR::execute ()
{
    OperandLocation destination = getOperandLocation (cpu_->registers ());

    if (!destination.isA<MemoryOperandLocation> ())
    {
        // Illegal instruction
        cpu_->setTrap (CpuData::TrapCondition::IllegalInstructionTrap);
        return true;
    }

    GeneralRegisters& registers = cpu_->registers ();
    u16 specifiedRegisterContents = registers[getRegisterNr ()];

    if (!mmu_->pushWord (specifiedRegisterContents))
        return false;

    if (cpu_->stackOverflow ())
        cpu_->setTrap (CpuData::TrapCondition::StackOverflow);

    registers[getRegisterNr ()] = registers[7];
    registers[7] = destination;

    return true;
}

#endif // _JSR_H_