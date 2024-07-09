#include "singleoperanddecoder.h"

SingleOperandDecoder::SingleOperandDecoder (CpuData* cpuData,
    CpuControl* cpuControl, MMU* mmu, SingleOperandInstruction* instr)
    :
    BaseOperandDecoder (cpuData, cpuControl, mmu),
    instr_ {instr}
{}

// Derive the location of the operand from the addressing mode and the
// register in the instruction and possibly the index and a memory address.
OperandLocation SingleOperandDecoder::getOperandLocation (GeneralRegisters& reg)
{
	return decodeOperand (instr_->getOperationCode (),
        Operand {instr_->getRegister (), instr_->getMode ()}, reg);
}
