#ifndef _KDF11_A_EXECUTOR_H_
#define _KDF11_A_EXECUTOR_H_

#include "kd/common/executor/executor.h"
#include "kd/kdf11/kdf11instruction/kdf11instruction.h"

namespace KDF11_A {

// This class contains the KDF11-A specific execution of some instructions.
// For most instructions the execution is forwarded to the Common::Executor.
//
class Executor : public Common::Executor
{
public:
    Executor (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu);

    // Single operand instructions
    bool operator() (KDF11Instruction::CLR& instr);
	bool operator() (KDF11Instruction::CLRB& instr);
	bool operator() (KDF11Instruction::COM& instr);
	bool operator() (KDF11Instruction::COMB& instr);
	bool operator() (KDF11Instruction::INC& instr);
	bool operator() (KDF11Instruction::INCB& instr);
	bool operator() (KDF11Instruction::DEC& instr);
	bool operator() (KDF11Instruction::DECB& instr);
	bool operator() (KDF11Instruction::NEG& instr);
	bool operator() (KDF11Instruction::NEGB& instr);
	bool operator() (KDF11Instruction::ASR& instr);
	bool operator() (KDF11Instruction::ASRB& instr);
	bool operator() (KDF11Instruction::ASL& instr);
	bool operator() (KDF11Instruction::ASLB& instr);
	bool operator() (KDF11Instruction::ROR& instr);
	bool operator() (KDF11Instruction::RORB& instr);
	bool operator() (KDF11Instruction::ROL& instr);
	bool operator() (KDF11Instruction::ROLB& instr);
	bool operator() (KDF11Instruction::SWAB& instr);
	bool operator() (KDF11Instruction::ADC& instr);
	bool operator() (KDF11Instruction::ADCB& instr);
	bool operator() (KDF11Instruction::SBC& instr);
	bool operator() (KDF11Instruction::SBCB& instr);
	bool operator() (KDF11Instruction::SXT& instr);
	bool operator() (KDF11Instruction::MFPS& instr);

	// Double operand instructions
	bool operator() (KDF11Instruction::MOV& instr);
	bool operator() (KDF11Instruction::MOVB& instr);
	bool operator() (KDF11Instruction::CMP& instr);
	bool operator() (KDF11Instruction::CMPB& instr);
	bool operator() (KDF11Instruction::ADD& instr);
	bool operator() (KDF11Instruction::SUB& instr);
	bool operator() (KDF11Instruction::BIT& instr);
	bool operator() (KDF11Instruction::BITB& instr);
	bool operator() (KDF11Instruction::BIC& instr);
	bool operator() (KDF11Instruction::BICB& instr);
	bool operator() (KDF11Instruction::BIS& instr);
	bool operator() (KDF11Instruction::BISB& instr);

	// EIS instructions, including JSR and XOR
	bool operator() (KDF11Instruction::XOR& instr);

	// No-operand instructions
	bool operator() (KDF11Instruction::HALT& instr);
	bool operator() (KDF11Instruction::WAIT& instr);
	bool operator() (KDF11Instruction::RESET& instr);
	bool operator() (KDF11Instruction::MFPD& instr);
    bool operator() (KDF11Instruction::MTPD& instr);
    bool operator() (KDF11Instruction::MFPT& instr);
};

} // namespace KDF11_A

#endif // _KDF11_A_EXECUTOR_H_