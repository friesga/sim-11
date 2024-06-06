#include "executor.h"

KD11_NA::Executor::Executor (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu)
    :
    cpuData_ {cpuData},
    cpuControl_ {cpuControl},
    mmu_ {mmu}
{}

 void KD11_NA::Executor::setPSW (ConditionCodes conditionCodes)
{
	cpuData_->setCC (conditionCodes);
}

 // Single operand instructions
bool KD11_NA::Executor::operator() (CommonInstruction::CLR& instr)
{
	return instr.execute ();
}

bool KD11_NA::Executor::operator() (CommonInstruction::CLRB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::COM& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::COMB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::INC& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::INCB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::DEC& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::DECB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::NEG& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::NEGB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::TST& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::TSTB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::ASR& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::ASRB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::ASL& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::ASLB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::ROR& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::RORB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::ROL& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::ROLB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::SWAB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::ADC& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::ADCB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::SBC& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::SBCB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::SXT& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::MFPS& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::MTPS& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::JMP& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::MARK& instr) { return instr.execute (); }

	// Double operand instructions
bool KD11_NA::Executor::operator() (CommonInstruction::MOV& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::MOVB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::CMP& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::CMPB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::ADD& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::SUB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BIT& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BITB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BIC& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BICB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BIS& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BISB& instr) { return instr.execute (); }

	// EIS instructions, including JSR and XOR
bool KD11_NA::Executor::operator() (CommonInstruction::JSR& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::MUL& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::DIV& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::ASH& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::ASHC& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::XOR& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::SOB& instr) { return instr.execute (); }

	// FIS format instructions
bool KD11_NA::Executor::operator() (CommonInstruction::RTS& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::FADD& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::FSUB& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::FMUL& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::FDIV& instr) { return instr.execute (); }

	// Branch instructions
bool KD11_NA::Executor::operator() (CommonInstruction::BR& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BNE& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BEQ& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BPL& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BMI& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BVC& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BVS& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BCC& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BCS& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BGE& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BLT& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BGT& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BLE& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BHI& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BLOS& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::EMT& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::TRAP& instr) { return instr.execute (); }

	// No-operand instructions
bool KD11_NA::Executor::operator() (CommonInstruction::HALT& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::WAIT& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::RTI& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::BPT& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::IOT& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::RESET& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::RTT& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::CCC& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::SCC& instr) { return instr.execute (); }
bool KD11_NA::Executor::operator() (CommonInstruction::Unused& instr) { return instr.execute (); }
