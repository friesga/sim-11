#include "kd11insn1.h"

bool KD11INSN1::getOperand (KD11CPU *cpu, u16 (&reg)[8], 
	Bitmask<OperandOptions> options, u16 &retValue)
{
	if (options & OperandOptions::Byte)
		return Instruction::getByteOperand (cpu, reg, options, mode, rn, retValue);
	else if (options & OperandOptions::Word)
		return Instruction::getWordOperand (cpu, reg, options, mode, rn, retValue);
	else
		throw (std::string("Missing getOperand option"));
}

bool KD11INSN1::getAddress (KD11CPU *cpu, u16 (&reg)[8], u16 &retValue)
{
	return Instruction::getAddress (cpu, reg, mode, rn, retValue);
}

bool KD11INSN1::putOperand (KD11CPU *cpu, u16 (&reg)[8], 
	Bitmask<OperandOptions> options, u16 value)
{
	if (options & OperandOptions::Byte)
		return Instruction::putByteOperand (cpu, reg, mode, rn, value);
	else if (options & OperandOptions::Word)
		return Instruction::putWordOperand (cpu, reg, mode, rn, value);
	else
		throw (std::string("Missing putOperand option"));
}