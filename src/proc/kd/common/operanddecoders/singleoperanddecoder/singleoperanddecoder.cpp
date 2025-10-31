#include "singleoperanddecoder.h"

SingleOperandDecoder::SingleOperandDecoder (CpuData* cpuData,
    CpuControl* cpuControl, MMU* mmu, SingleOperandInstruction* instr)
    :
    BaseOperandDecoder (cpuData, cpuControl, mmu),
    instr_ {instr}
{}

// Derive the location of the operand from the addressing mode and the
// register in the instruction and possibly the index and a memory address.
// 
// Make sure the operand location is determined just once to avoid
// autoincrement or autodecrement being applied to a register twice.
//
OperandLocation SingleOperandDecoder::getOperandLocation (GeneralRegisters& reg)
{
    if (!operandLocation_.isValid ())
    {
        operandLocation_ = decodeOperand (instr_->getOperationCode (),
            Operand {instr_->getRegister (), instr_->getMode ()}, reg);
    }
    return operandLocation_;
}
