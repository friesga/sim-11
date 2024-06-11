#include "executor.h"

KDF11_A::Executor::Executor::Executor (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu)
    :
    Common::Executor (cpuData, cpuControl, mmu)
{}

bool KDF11_A::Executor::operator() (KDF11Instruction::CLR& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::CLRB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::COM& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::COMB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::INC& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::INCB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::DEC& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::DECB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::NEG& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::NEGB& instr) { return instr.execute (); }

bool KDF11_A::Executor::operator() (KDF11Instruction::ASR& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::ASRB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::ASL& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::ASLB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::ROR& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::RORB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::ROL& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::ROLB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::SWAB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::ADC& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::ADCB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::SBC& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::SBCB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::SXT& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::MFPS& instr) { return instr.execute (); }

	// Double operand instructions
bool KDF11_A::Executor::operator() (KDF11Instruction::MOV& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::MOVB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::CMP& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::CMPB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::ADD& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::SUB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::BIT& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::BITB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::BIC& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::BICB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::BIS& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::BISB& instr) { return instr.execute (); }

	// EIS instructions, including JSR and XOR
bool KDF11_A::Executor::operator() (KDF11Instruction::XOR& instr) { return instr.execute (); }

	// No-operand instructions
bool KDF11_A::Executor::operator() (KDF11Instruction::HALT& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::WAIT& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::RESET& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::MFPD& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::MTPD& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (KDF11Instruction::MFPT& instr) { return instr.execute (); }
