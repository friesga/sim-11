#ifndef _JMP_H_
#define _JMP_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"
#include "trace/trace.h"

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
//
class CommonInstruction::JMP : public SingleOperandInstruction, public WithFactory<JMP>
{
public:
    JMP (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::JMP::JMP (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::JMP::execute ()
{
    operandLocation_ = getOperandLocation (cpu_->registers ());
    if (operandLocation_.isA<MemoryOperandLocation> ())
    {
        cpu_->registers ()[7] = operandLocation_;
        return CpuData::Trap::None;
    }

    // Illegal instruction
    trace.trap (TrapRecordType::TRAP_RADDR, 04);
    return CpuData::Trap::BusError;
}


#endif // _JMP_H_