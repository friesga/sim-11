#ifndef _SINGLEOPERANDDECODER_H_
#define _SINGLEOPERANDDECODER_H_

#include "proc/kd/common/operanddecoders/baseoperanddecoder/baseoperanddecoder.h"
#include "proc/kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "proc/kd/include/cpudata.h"
#include "proc/include/cpucontrol.h"
#include "proc/kd/include/mmu.h"

class SingleOperandDecoder : BaseOperandDecoder
{
public:
    SingleOperandDecoder (CpuData* cpuData, Interfaces::CpuController* cpuControl, MMU* mmu,
		SingleOperandInstruction* instr);

	OperandLocation getOperandLocation (GeneralRegisters &reg);
    template <typename T> bool readOperand (T *operand);
	template <typename T> bool writeOperand (T operand);

private:
	SingleOperandInstruction* instr_ {nullptr};
	OperandLocation operandLocation_ {};
};

// The functions below are templated for bytes (type u8 or CondData<u8>) and
// words (type u16 and CondData<u16>). Trying to use the functions for other
// types will result in compilation errors.
template <typename T>
bool SingleOperandDecoder::readOperand (T *operand)
{
	OperandLocation operandLocation =
		getOperandLocation (cpuData_->registers ());

	*operand = operandLocation.contents<T> ();
	return (*operand).hasValue ();
}

template <typename T>
bool SingleOperandDecoder::writeOperand (T operand)
{
	OperandLocation operandLocation =
		getOperandLocation (cpuData_->registers ());
	
	return operandLocation_.write<T> (operand);
}

#endif // _SINGLEOPERANDDECODER_H_