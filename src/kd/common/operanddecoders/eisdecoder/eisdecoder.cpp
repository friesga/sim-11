#include "eisdecoder.h"

EisDecoder::EisDecoder (CpuData* cpuData, CpuControl* cpuControl,
		MMU* mmu,EisInstruction* instruction)
    :
	BaseOperandDecoder (cpuData, cpuControl, mmu),
    instr_ {instruction}
{}

// Derive the location of the operand from the instruction
// 
// Make sure the operand location is determined just once to avoid
// autoincrement or autodecrement being applied to a register twice.
//
OperandLocation EisDecoder::getOperandLocation (GeneralRegisters& reg)
{
	if (!operandLocation_.isValid ())
	{
		operandLocation_ = decodeOperand (instr_->getOperationCode (),
			Operand {instr_->getOperandRegister (), instr_->getOperandMode ()}, reg);
	}
	return operandLocation_;
}

// The operand in the EIS instruction format is either a source or
// a destination operand.
bool EisDecoder::readOperand (CondData<u16> *source)
{
	OperandLocation operandLocation = 
		getOperandLocation (cpuData_->registers ());
    *source = operandLocation.contents<CondData<u16>> ();
	return (*source).hasValue ();
}

bool EisDecoder::writeOperand (u16 operand)
{
	OperandLocation operandLocation = 
			getOperandLocation (cpuData_->registers ());
	
	return operandLocation.write (operand);
}
