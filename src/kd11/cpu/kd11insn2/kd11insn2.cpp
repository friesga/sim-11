#include "kd11insn2.h"

bool KD11INSN2::getSourceOperand (KD11CPU *cpu, u16 (&reg)[8], 
		Bitmask<OperandOptions> options, u16 &retValue)
{
	if (options & OperandOptions::Byte)
		return Instruction::getByteOperand (cpu, reg, options,
			src_mode, src_rn, retValue);
	else if (options & OperandOptions::Word)
		return Instruction::getWordOperand (cpu, reg, options, 
			src_mode, src_rn, retValue);
	else
		throw (std::string("Missing getSourceOperand option"));
}

bool KD11INSN2::getDestOperand (KD11CPU *cpu, u16 (&reg)[8], 
		Bitmask<OperandOptions> options, u16 &retValue)
{
	if (options & OperandOptions::Byte)
		return Instruction::getByteOperand (cpu, reg, options,
			dst_mode, dst_rn, retValue);
	else if (options & OperandOptions::Word)
		return Instruction::getWordOperand (cpu, reg, options, 
			dst_mode, dst_rn, retValue);
	else
		throw (std::string("Missing getDestOperand option"));
}

bool KD11INSN2::putDestOperand (KD11CPU *cpu, u16 (&reg)[8], 
	Bitmask<OperandOptions> options, u16 value)
{
	if (options & OperandOptions::Byte)
		return Instruction::putByteOperand (cpu, reg, dst_mode, dst_rn, value);
	else if (options & OperandOptions::Word)
		return Instruction::putWordOperand (cpu, reg, dst_mode, dst_rn, value);
	else
		throw (std::string("Missing putDestOperand option"));
}
