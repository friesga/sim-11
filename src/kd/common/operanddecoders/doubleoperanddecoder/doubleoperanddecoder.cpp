#include "doubleoperanddecoder.h"

// It would be nice if source and destination operand locations and operands
// could be retrieved in the constructor. This is not possible however, as
// not for all instructions the destination operand has to be retrieved and
// we need a return value for the retrieval.
// 
// Initialize the instr struct with the first member
DoubleOperandDecoder::DoubleOperandDecoder (CpuData* cpuData, CpuControl* cpuControl,
		MMU* mmu, DoubleOperandInstruction* instruction)
	:
	BaseOperandDecoder (cpuData, cpuControl, mmu),
	instr_ {instruction}
{}

OperandLocation DoubleOperandDecoder::getSourceOperandLocation (GeneralRegisters &reg)
{
	return decodeOperand (instr_->getOperationCode (),
		Operand {instr_->getSourceRegister(), instr_->getSourceMode ()}, reg);
}

OperandLocation DoubleOperandDecoder::getDestinationOperandLocation (GeneralRegisters &reg)
{
	return decodeOperand (instr_->getOperationCode (),
		Operand {instr_->getDestinationRegister (), instr_->getDestinationMode ()}, reg);
}

// On most PDP-11's the source operand location is determined before the
// destination operand location. This is also the case for the KD11-NA.
void DoubleOperandDecoder::getOperandLocations (GeneralRegisters &reg)
{
	sourceOperandLocation_ = getSourceOperandLocation (reg);
	destinationOperandLocation_ = getDestinationOperandLocation (reg);
}

bool DoubleOperandDecoder::writeDestinationOperands8 (s8 operand)
{
    // If the destination mode is 0 (Register) the regular operand processing
    // is bypassed and the signed eight bit value s8 is directly written into
    // the register, causing sign extension in the register.
    if (destinationOperandLocation_.isA<RegisterOperandLocation> ())
        cpuData_->registers ()[destinationOperandLocation_] = operand;
    else
        if (!destinationOperandLocation_.write<u8> (operand))
            return false;

    return true;
}
