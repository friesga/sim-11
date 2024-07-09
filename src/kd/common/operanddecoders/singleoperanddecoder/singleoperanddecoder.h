#ifndef _SINGLEOPERANDDECODER_H_
#define _SINGLEOPERANDDECODER_H_

#include "kd/common/operanddecoders/baseoperanddecoder/baseoperanddecoder.h"
#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/include/mmu.h"

class SingleOperandDecoder : BaseOperandDecoder
{
public:
    SingleOperandDecoder (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu,
		SingleOperandInstruction* instr);

	OperandLocation getOperandLocation (GeneralRegisters &reg);
    template <typename T> bool readOperand (T *operand);
	template <typename T> bool writeOperand (T operand);

protected:
	// The operand location is protected as some instructions (notably the JMP
	// and MFPS instructions) are special cases and need access to the location.
	OperandLocation operandLocation_ {};

private:
	SingleOperandInstruction* instr_ {nullptr};
};

// The functions below are templated for bytes (type u8 or CondData<u8>) and
// words (type u16 and CondData<u16>). Trying to use the functions for other
// types will result in compilation errors.
template <typename T>
bool SingleOperandDecoder::readOperand (T *operand)
{
	operandLocation_ =  getOperandLocation (cpuData_->registers ());
    *operand = operandLocation_.contents<T> ();
	return (*operand).hasValue ();
}

template <typename T>
bool SingleOperandDecoder::writeOperand (T operand)
{
	if (!operandLocation_.isValid ())
	{
		operandLocation_ = 
			getOperandLocation (cpuData_->registers ());
	}
	
	return operandLocation_.write<T> (operand);
}

#endif // _SINGLEOPERANDDECODER_H_