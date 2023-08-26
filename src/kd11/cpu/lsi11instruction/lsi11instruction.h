#ifndef _LSI11INSTRUCTION_H_
#define _LSI11INSTRUCTION_H_

#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "../operand/operand.h"

#include <string>

using std::string;

class LSI11Instruction
{
public:
	// All derived classes will have to provide an execute function
	// ToDo: Make execute() a pute virtual function
	virtual CpuData::Trap execute () { return CpuData::Trap::None; };
	string mnemonic ();

protected:
	CpuData *cpu_;

	LSI11Instruction (CpuData *cpu);
	OperandLocation decodeOperand (Operand operand, u16 (&reg)[8]);
	bool isByteInstruction ();

	// All derived classes have to provide a function returning the operation
	// code of the instruction. This detour to get at the instruction's
	// opcode is necessary as the decoding of the instruction takes places
	// after LSII11Instruction's constructor is called.
	virtual u16 getOperationCode () = 0;

	constexpr bool isSet (u16 x);
	constexpr void setConditionCode (u16 x);
	constexpr void clearConditionCode (u16 x);
	constexpr void setConditionCodeIf_ClearElse (u16 x, bool v);
};

constexpr bool LSI11Instruction::isSet (u16 x)
{
	return (cpu_->psw () & x) ? true : false;
}

constexpr void LSI11Instruction::setConditionCode (u16 x)
{
	cpu_->psw () |= x;
}

constexpr void LSI11Instruction::clearConditionCode (u16 x)
{
	cpu_->psw () &= ~x;
}

constexpr void LSI11Instruction::setConditionCodeIf_ClearElse (u16 x, bool condition)
{
	if (condition)
		setConditionCode (x);
	else
		clearConditionCode (x);
}

#endif _LSI11INSTRUCTION_H_