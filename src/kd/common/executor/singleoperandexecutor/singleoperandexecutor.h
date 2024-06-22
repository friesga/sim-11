#ifndef _SINGLEOPERANDEXECUTOR_H_
#define _SINGLEOPERANDEXECUTOR_H_

#include "kd/common/instructions/instructions.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/include/mmu.h"
#include "kd/common/executor/baseexecutor/baseexecutor.h"

#include <functional>

using std::function;

class SingleOperandExecutor : public BaseExecutor
{
public:
    SingleOperandExecutor (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu);
	bool execute (function<bool(SingleOperandInstruction* instr)> instrFunction);

protected:
	// The operand location is protected as some instructions (notably the JMP
	// and MFPS instructions) are special cases and need access to the location.
	OperandLocation operandLocation_ {};

private:
    CpuData* cpuData_;
    CpuControl* cpuControl_;
    MMU* mmu_;
	SingleOperandInstruction* instr_ {nullptr};

	OperandLocation getOperandLocation (GeneralRegisters &reg);
    template <typename T> bool readOperand (T *operand);
	template <typename T> bool writeOperand (T operand);
};

// The functions below are templated for bytes (type u8 or CondData<u8>) and
// words (type u16 and CondData<u16>). Trying to use the functions for other
// types will result in compilation errors.
template <typename T>
bool SingleOperandExecutor::readOperand (T *operand)
{
	operandLocation_ =  getOperandLocation (cpuData_->registers ());
    *operand = operandLocation_.contents<T> ();
	return (*operand).hasValue ();
}

template <typename T>
bool SingleOperandExecutor::writeOperand (T operand)
{
	if (!operandLocation_.isValid ())
	{
		operandLocation_ = 
			getOperandLocation (cpuData_->registers ());
	}
	
	return operandLocation_.write<T> (operand);
}

#endif // _SINGLEOPERANDEXECUTOR_H_