#include "eisinstruction.h"
#include "kd/common/operand/operand.h"

EisInstruction::EisInstruction (CpuData* cpu, CpuControl* cpuControl,
		MMU* mmu, u16 instruction)
    :
	LSI11Instruction (cpu, cpuControl, mmu),
    instr_ {instruction}
{}

// Derive the location of the operand from the instruction
OperandLocation EisInstruction::getOperandLocation (GeneralRegisters& reg)
{
	return decodeOperand (Operand {instr_.decoded.operandRegNr,
		instr_.decoded.operandMode}, reg);
}

// The operand in the EIS instruction format is either a source or
// a destination operand.
bool EisInstruction::readOperand (CondData<u16> *source)
{
	operandLocation_ = 
		getOperandLocation (cpu_->registers ());
    *source = operandLocation_.contents<CondData<u16>> ();
	return (*source).hasValue ();
}

bool EisInstruction::writeOperand (u16 operand)
{
	if (!operandLocation_.isValid ())
	{
		operandLocation_ = 
			getOperandLocation (cpu_->registers ());
	}
	
	return operandLocation_.write (operand);
}

// Return the instruction's operation code
u16 EisInstruction::getOperationCode ()
{
	return instr_.decoded.opcode;
}

u16 EisInstruction::getRegisterNr ()
{
	return instr_.decoded.registerNr;
}