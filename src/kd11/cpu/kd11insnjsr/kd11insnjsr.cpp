#include "kd11insnjsr.h"

bool KD11INSNJSR::getOperand (KD11CPU *cpu, u16 (&reg)[8], 
	Bitmask<OperandOptions> options, u16 &retValue)
{
	if (options & OperandOptions::Byte)
		return Instruction::getByteOperand (cpu, reg, options, mode, rn, retValue);
	else if (options & OperandOptions::Word)
		return Instruction::getWordOperand (cpu, reg, options, mode, rn, retValue);
	else
		throw (std::string("Missing getOperand option"));
}

bool KD11INSNJSR::getAddress (KD11CPU *cpu, u16 (&reg)[8], u16 &retValue)
{
	return Instruction::getAddress (cpu, reg, mode, rn, retValue);
}

bool KD11INSNJSR::putOperand (KD11CPU *cpu, u16 (&reg)[8], u16 value)
{
	return Instruction::putWordOperand (cpu, reg, mode, rn, value);
}
