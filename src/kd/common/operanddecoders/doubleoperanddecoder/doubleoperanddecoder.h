#ifndef _DOUBLEOPERANDDECODER_H_
#define _DOUBLEOPERANDDECODER_H_

#include "kd/common/operanddecoders/baseoperanddecoder/baseoperanddecoder.h"
#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/include/mmu.h"

class DoubleOperandDecoder : public BaseOperandDecoder
{
public:
	DoubleOperandDecoder (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu,
		DoubleOperandInstruction* instruction);

	OperandLocation getSourceOperandLocation (GeneralRegisters &reg);
	OperandLocation getDestinationOperandLocation (GeneralRegisters &reg);
	void getOperandLocations (GeneralRegisters &reg);
	template <typename T> bool readSourceOperand (T *source);
	template <typename T> bool readDestinationOperand (T *destination);
	template <typename T> bool writeDestinationOperand (T operand);
	bool writeDestinationOperands8 (s8 operand);

private:
	// The source and destination operand locations are defined as class
	// members as for the KD11-F in some cases the destination operand
	// location has to be determined before the source operand is retrieved.
	OperandLocation sourceOperandLocation_ {};
	OperandLocation destinationOperandLocation_ {};

	DoubleOperandInstruction* instr_ {nullptr};
};

// The functions below are templated for bytes (type u8 or CondData<u8>) and
// words (type u16 and CondData<u16>). Trying to use the functions for other
// types will result in compilation errors.
template <typename T>
bool DoubleOperandDecoder::readSourceOperand (T *source)
{
	if (!sourceOperandLocation_.isValid ())
	{
		sourceOperandLocation_ = 
			getSourceOperandLocation (cpuData_->registers ());
	}

    *source = sourceOperandLocation_.contents<T> ();
	return (*source).hasValue ();
}

template <typename T>
bool DoubleOperandDecoder::readDestinationOperand (T *destination)
{
	OperandLocation destinationOperandLocation = 
			getDestinationOperandLocation (cpuData_->registers ());

    *destination = destinationOperandLocation.contents<T> ();
	return (*destination).hasValue ();
}

// For most instructions the destination operand location will have been
// determined when the destion operand has been retrieved. Some instructions
// however just write the destination operand. In these cases the operand
// location still has to be determined.
template <typename T>
bool DoubleOperandDecoder::writeDestinationOperand (T operand)
{
	OperandLocation destinationOperandLocation = 
			getDestinationOperandLocation (cpuData_->registers ());
	
	return destinationOperandLocation.write<T> (operand);
}

#endif // _DOUBLE_H_