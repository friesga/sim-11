#include "eisdecoder.h"

EisDecoder::EisDecoder (CpuData* cpuData, CpuControl* cpuControl,
		MMU* mmu,EisInstruction* instruction)
    :
	BaseOperandDecoder (cpuData, cpuControl, mmu),
    instr_ {instruction}
{}

// Derive the location of the operand from the instruction
OperandLocation EisDecoder::getOperandLocation (GeneralRegisters& reg)
{
	return decodeOperand (instr_->getOperationCode (), 
		Operand {instr_->getOperandRegister(), instr_->getOperandMode ()}, reg);
}

// The operand in the EIS instruction format is either a source or
// a destination operand.
bool EisDecoder::readOperand (CondData<u16> *source)
{
	operandLocation_ = 
		getOperandLocation (cpuData_->registers ());
    *source = operandLocation_.contents<CondData<u16>> ();
	return (*source).hasValue ();
}

bool EisDecoder::writeOperand (u16 operand)
{
	if (!operandLocation_.isValid ())
	{
		operandLocation_ = 
			getOperandLocation (cpuData_->registers ());
	}
	
	return operandLocation_.write (operand);
}
