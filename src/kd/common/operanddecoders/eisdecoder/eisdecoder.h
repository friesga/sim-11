#ifndef _EISDECODER_H_
#define _EISDECODER_H_

#include "kd/common/operanddecoders/baseoperanddecoder/baseoperanddecoder.h"
#include "kd/common/instructionformats/eisinstruction/eisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/include/mmu.h"

// The EIS instruction format is used by the EIS instructions MUL, DIV, ASH
// and ASHC instruction and additionally by the JSR and XOR instructions.
class EisDecoder : public BaseOperandDecoder
{
public:
	EisDecoder (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu,
		EisInstruction* instruction);

	OperandLocation getOperandLocation (GeneralRegisters &reg);
	bool readOperand (CondData<u16> *source);
	bool writeOperand (u16 operand);

private:
	EisInstruction* instr_ {nullptr};
	OperandLocation operandLocation_ {};
};


#endif // !_EISDECODER_H_
