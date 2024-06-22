#include "singleoperandexecutor.h"

SingleOperandExecutor::SingleOperandExecutor (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu)
    :
    BaseExecutor (cpuData, mmu),
    cpuData_ {cpuData},
    cpuControl_ {cpuControl},
    mmu_ {mmu}
{}

// Derive the location of the operand from the addressing mode and the
// register in the instruction and possibly the index and a memory address.
OperandLocation SingleOperandExecutor::getOperandLocation (GeneralRegisters& reg)
{
	return decodeOperand (Operand {instr_->getRegister (), instr_->getMode ()}, reg);
}

bool SingleOperandExecutor::execute (function<bool(SingleOperandInstruction* instr)> instrFunction)
{
    return instrFunction ();
}