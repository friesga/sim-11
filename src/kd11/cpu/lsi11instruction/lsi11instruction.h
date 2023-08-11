#ifndef _LSI11INSTRUCTION_H_
#define _LSI11INSTRUCTION_H_

#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "../operand/operand.h"

class LSI11Instruction
{
public:
	// All derived classes will have to provide an execute function
	// ToDo: Make execute() a pute virtual function
	virtual CpuData::Trap execute () { return CpuData::Trap::None; };

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
};

#endif _LSI11INSTRUCTION_H_