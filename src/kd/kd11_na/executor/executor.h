#ifndef _KD11_NA_EXECUTOR_H_
#define _KD11_NA_EXECUTOR_H_

#include "kd/common/executor/executor.h"

namespace KD11_NA {

// This class contains the KD11-NA specific execution of some instructions.
// For most instructions the execution is forwarded to the Common::Executor.
//
class Executor : public Common::Executor
{
public:
    Executor (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu);

    // Single operand instructions
    bool operator() (CLR& instr);
	bool operator() (CLRB& instr);
	bool operator() (COM& instr);
	bool operator() (COMB& instr);
	bool operator() (INC& instr);
	bool operator() (INCB& instr);
	bool operator() (DEC& instr);
	bool operator() (DECB& instr);
	bool operator() (NEG& instr);
	bool operator() (NEGB& instr);
	bool operator() (TST& instr);
	bool operator() (TSTB& instr);
	bool operator() (ASR& instr);
	bool operator() (ASRB& instr);
	bool operator() (ASL& instr);
	bool operator() (ASLB& instr);
	bool operator() (ROR& instr);
	bool operator() (RORB& instr);
	bool operator() (ROL& instr);
	bool operator() (ROLB& instr);
	bool operator() (SWAB& instr);
	bool operator() (ADC& instr);
	bool operator() (ADCB& instr);
	bool operator() (SBC& instr);
	bool operator() (SBCB& instr);
	bool operator() (SXT& instr);
	bool operator() (MFPS& instr);
	bool operator() (MTPS& instr);
	bool operator() (JMP& instr);
	bool operator() (MARK& instr);

	// Double operand instructions
	bool operator() (MOV& instr);
	bool operator() (MOVB& instr);
	bool operator() (CMP& instr);
	bool operator() (CMPB& instr);
	bool operator() (ADD& instr);
	bool operator() (SUB& instr);
	bool operator() (BIT& instr);
	bool operator() (BITB& instr);
	bool operator() (BIC& instr);
	bool operator() (BICB& instr);
	bool operator() (BIS& instr);
	bool operator() (BISB& instr);

	// EIS instructions, including JSR and XOR
	bool operator() (JSR& instr);
	bool operator() (MUL& instr);
	bool operator() (DIV& instr);
	bool operator() (ASH& instr);
	bool operator() (ASHC& instr);
	bool operator() (XOR& instr);
	bool operator() (SOB& instr);

	// FIS format instructions
	bool operator() (RTS& instr);
	bool operator() (FADD& instr);
	bool operator() (FSUB& instr);
	bool operator() (FMUL& instr);
	bool operator() (FDIV& instr);

	// Branch instructions
	bool operator() (BR& instr);
	bool operator() (BNE& instr);
	bool operator() (BEQ& instr);
	bool operator() (BPL& instr);
	bool operator() (BMI& instr);
	bool operator() (BVC& instr);
	bool operator() (BVS& instr);
	bool operator() (BCC& instr);
	bool operator() (BCS& instr);
	bool operator() (BGE& instr);
	bool operator() (BLT& instr);
	bool operator() (BGT& instr);
	bool operator() (BLE& instr);
	bool operator() (BHI& instr);
	bool operator() (BLOS& instr);
	bool operator() (EMT& instr);
	bool operator() (TRAP& instr);

	// No-operand instructions
	bool operator() (HALT& instr);
	bool operator() (WAIT& instr);
	bool operator() (RTI& instr);
	bool operator() (BPT& instr);
	bool operator() (IOT& instr);
	bool operator() (RESET& instr);
	bool operator() (RTT& instr);
	bool operator() (CCC& instr);
	bool operator() (SCC& instr);
	bool operator() (Unused& instr);
};

} // namespace KD11_NA

#endif // _KD11_NA_EXECUTOR_H_