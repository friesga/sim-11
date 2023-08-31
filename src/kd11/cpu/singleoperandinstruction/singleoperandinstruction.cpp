#include "singleoperandinstruction.h"
#include "../operand/operand.h"

// Initialize the instr struct with the first member
SingleOperandInstruction::SingleOperandInstruction (CpuData *cpu, u16 instruction)
	:
	LSI11Instruction (cpu),
	instr_ {instruction}
{}

// Derive the location of the operand from the addressing mode and the
// register in the instruction and possibly the index and a memory address.
OperandLocation SingleOperandInstruction::getOperandLocation (u16 (&reg)[8])
{
	return decodeOperand (Operand {instr_.decoded.rn, instr_.decoded.mode}, reg);
}

// Return the instruction's operation code
u16 SingleOperandInstruction::getOperationCode ()
{
	return instr_.decoded.opcode;
}

bool SingleOperandInstruction::readOperand (CondData<u16> *operand)
{
	operandLocation_ =  getOperandLocation (cpu_->registers ());
    *operand = operandLocation_.contents<u16> ();
	return (*operand).hasValue ();
}

bool SingleOperandInstruction::readOperand (CondData<u8> *operand)
{
	operandLocation_ =  getOperandLocation (cpu_->registers ());
    *operand = operandLocation_.contents<u8> ();
	return (*operand).hasValue ();
}

bool SingleOperandInstruction::writeOperand (u16 operand)
{
	if (!operandLocation_.isValid ())
	{
		operandLocation_ = 
			getOperandLocation (cpu_->registers ());
	}
	
	return operandLocation_.write (operand);
}

bool SingleOperandInstruction::writeOperand (u8 operand)
{
	if (!operandLocation_.isValid ())
	{
		operandLocation_ = 
			getOperandLocation (cpu_->registers ());
	}
	
	return operandLocation_.write<u8> (operand);
}

