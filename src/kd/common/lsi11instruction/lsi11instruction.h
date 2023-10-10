#ifndef _LSI11INSTRUCTION_H_
#define _LSI11INSTRUCTION_H_

#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "kd/kd11_na/cpu/operand/operand.h"

#include <string>

using std::string;

// The processor status word in the LSI-11 is a composite of (1) the 4 PDP-11
// condition codes (N,Z,V,C) [bits 0:3] and (2) the Trace Bit [bit 4] and (3)
// the Interrupt Enable bit [bit 7]. (EK-KUV11-TM-001 pg. 1-9). 
// Processor Status Word bits 5 and 6 are reserved (EK-KUV11-TM-001
// Figure 2-5). These bits can be set and cleared by the MTPS instruction
// however. At least every FIS instruction resets bits 5 and 6. This
// behaviour is confirmed by the VKAAC0 (Basic Intruction Tests) and VKACC0
// diagnostics.
#define	PSW_C			_BV(0)
#define	PSW_V			_BV(1)
#define	PSW_Z			_BV(2)
#define	PSW_N			_BV(3)
#define	PSW_T			_BV(4)
#define	PSW_PRIO		_BV(7)
#define	PSW_PRIORITY	(_BV(7) | _BV(6) | _BV(5))

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

#endif // _LSI11INSTRUCTION_H_