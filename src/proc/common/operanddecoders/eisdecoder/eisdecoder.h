#ifndef _EISDECODER_H_
#define _EISDECODER_H_

#include "proc/common/operanddecoders/baseoperanddecoder/baseoperanddecoder.h"
#include "proc/common/instructionformats/eisinstruction/eisinstruction.h"
#include "proc/include/cpudata.h"
#include "proc/include/cpucontroller.h"
#include "proc/include/mmu.h"

// The EIS instruction format is used by the EIS instructions MUL, DIV, ASH
// and ASHC instruction and additionally by the JSR and XOR instructions.
class EisDecoder : public BaseOperandDecoder
{
public:
	EisDecoder (CpuData* cpuData, Interfaces::CpuController* cpuController, MMU* mmu,
		EisInstruction* instruction);

	OperandLocation getOperandLocation (GeneralRegisters &reg);
	bool readOperand (CondData<u16> *source);
	bool writeOperand (u16 operand);

private:
	EisInstruction* instr_ {nullptr};
	OperandLocation operandLocation_ {};
};


#endif // !_EISDECODER_H_
