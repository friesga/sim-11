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
    bool operator() (CommonInstruction::CLR& instr);
	bool operator() (CommonInstruction::CLRB& instr);
	bool operator() (CommonInstruction::COM& instr);
	bool operator() (CommonInstruction::COMB& instr);
	bool operator() (CommonInstruction::INC& instr);
	bool operator() (CommonInstruction::INCB& instr);
	bool operator() (CommonInstruction::DEC& instr);
	bool operator() (CommonInstruction::DECB& instr);
	bool operator() (CommonInstruction::NEG& instr);
	bool operator() (CommonInstruction::NEGB& instr);
	bool operator() (CommonInstruction::TST& instr);
	bool operator() (CommonInstruction::TSTB& instr);
	bool operator() (CommonInstruction::ASR& instr);
	bool operator() (CommonInstruction::ASRB& instr);
	bool operator() (CommonInstruction::ASL& instr);
	bool operator() (CommonInstruction::ASLB& instr);
	bool operator() (CommonInstruction::ROR& instr);
	bool operator() (CommonInstruction::RORB& instr);
	bool operator() (CommonInstruction::ROL& instr);
	bool operator() (CommonInstruction::ROLB& instr);
	bool operator() (CommonInstruction::SWAB& instr);
	bool operator() (CommonInstruction::ADC& instr);
	bool operator() (CommonInstruction::ADCB& instr);
	bool operator() (CommonInstruction::SBC& instr);
	bool operator() (CommonInstruction::SBCB& instr);
	bool operator() (CommonInstruction::SXT& instr);
	bool operator() (CommonInstruction::MFPS& instr);
	bool operator() (CommonInstruction::MTPS& instr);
	bool operator() (CommonInstruction::JMP& instr);
	bool operator() (CommonInstruction::MARK& instr);

	// Double operand instructions
	bool operator() (CommonInstruction::MOV& instr);
	bool operator() (CommonInstruction::MOVB& instr);
	bool operator() (CommonInstruction::CMP& instr);
	bool operator() (CommonInstruction::CMPB& instr);
	bool operator() (CommonInstruction::ADD& instr);
	bool operator() (CommonInstruction::SUB& instr);
	bool operator() (CommonInstruction::BIT& instr);
	bool operator() (CommonInstruction::BITB& instr);
	bool operator() (CommonInstruction::BIC& instr);
	bool operator() (CommonInstruction::BICB& instr);
	bool operator() (CommonInstruction::BIS& instr);
	bool operator() (CommonInstruction::BISB& instr);

	// EIS instructions, including JSR and XOR
	bool operator() (CommonInstruction::JSR& instr);
	bool operator() (CommonInstruction::MUL& instr);
	bool operator() (CommonInstruction::DIV& instr);
	bool operator() (CommonInstruction::ASH& instr);
	bool operator() (CommonInstruction::ASHC& instr);
	bool operator() (CommonInstruction::XOR& instr);
	bool operator() (CommonInstruction::SOB& instr);

	// FIS format instructions
	bool operator() (CommonInstruction::RTS& instr);
	bool operator() (CommonInstruction::FADD& instr);
	bool operator() (CommonInstruction::FSUB& instr);
	bool operator() (CommonInstruction::FMUL& instr);
	bool operator() (CommonInstruction::FDIV& instr);

	// Branch instructions
	bool operator() (CommonInstruction::BR& instr);
	bool operator() (CommonInstruction::BNE& instr);
	bool operator() (CommonInstruction::BEQ& instr);
	bool operator() (CommonInstruction::BPL& instr);
	bool operator() (CommonInstruction::BMI& instr);
	bool operator() (CommonInstruction::BVC& instr);
	bool operator() (CommonInstruction::BVS& instr);
	bool operator() (CommonInstruction::BCC& instr);
	bool operator() (CommonInstruction::BCS& instr);
	bool operator() (CommonInstruction::BGE& instr);
	bool operator() (CommonInstruction::BLT& instr);
	bool operator() (CommonInstruction::BGT& instr);
	bool operator() (CommonInstruction::BLE& instr);
	bool operator() (CommonInstruction::BHI& instr);
	bool operator() (CommonInstruction::BLOS& instr);
	bool operator() (CommonInstruction::EMT& instr);
	bool operator() (CommonInstruction::TRAP& instr);

	// No-operand instructions
	bool operator() (CommonInstruction::HALT& instr);
	bool operator() (CommonInstruction::WAIT& instr);
	bool operator() (CommonInstruction::RTI& instr);
	bool operator() (CommonInstruction::BPT& instr);
	bool operator() (CommonInstruction::IOT& instr);
	bool operator() (CommonInstruction::RESET& instr);
	bool operator() (CommonInstruction::RTT& instr);
	bool operator() (CommonInstruction::CCC& instr);
	bool operator() (CommonInstruction::SCC& instr);
	bool operator() (CommonInstruction::Unused& instr);
};

} // namespace KD11_NA

#endif // _KD11_NA_EXECUTOR_H_