#ifndef _COMMON_EXECUTOR_H_
#define _COMMON_EXECUTOR_H_

#include "kd/common/instructions/instructions.h"
#include "singleoperandexecutor/singleoperandexecutor.h"

namespace Common {

// The PDP-11 family comprises several processor types. Most instructions
// for these processor types share their functionality but some instructions
// show minor differences in their execution, see e.g. Appendix C in the
// KDF11-BA Cpu Module User's Guide.
//
// Instructions with identical functionality for the different processors
// are defined in this class. Processors with a different execution for a
// specific instruction are defined in their own Executor class. The link
// from the specific executor function to the corresponding function in this
// class is created explicitly to make clear which version of the instruction
// is executed for a specific processor. The additional function call will
// be optimized away by the compiler.
//
class Executor
{
public:
    Executor (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu);

    // Single operand instructions
    bool execute (CLR& instr);
	bool execute (CLRB& instr);
	bool execute (COM& instr);
	bool execute (COMB& instr);
	bool execute (INC& instr);
	bool execute (INCB& instr);
	bool execute (DEC& instr);
	bool execute (DECB& instr);
	bool execute (NEG& instr);
	bool execute (NEGB& instr);
	bool execute (TST& instr);
	bool execute (TSTB& instr);
	bool execute (ASR& instr);
	bool execute (ASRB& instr);
	bool execute (ASL& instr);
	bool execute (ASLB& instr);
	bool execute (ROR& instr);
	bool execute (RORB& instr);
	bool execute (ROL& instr);
	bool execute (ROLB& instr);
	bool execute (SWAB& instr);
	bool execute (ADC& instr);
	bool execute (ADCB& instr);
	bool execute (SBC& instr);
	bool execute (SBCB& instr);
	bool execute (SXT& instr);
	bool execute (MFPS& instr);
	bool execute (MTPS& instr);
	bool execute (JMP& instr);
	bool execute (MARK& instr);

	// Double operand instructions
	bool execute (MOV& instr);
	bool execute (MOVB& instr);
	bool execute (CMP& instr);
	bool execute (CMPB& instr);
	bool execute (ADD& instr);
	bool execute (SUB& instr);
	bool execute (BIT& instr);
	bool execute (BITB& instr);
	bool execute (BIC& instr);
	bool execute (BICB& instr);
	bool execute (BIS& instr);
	bool execute (BISB& instr);

	// EIS instructions, including JSR and XOR
	bool execute (JSR& instr);
	bool execute (MUL& instr);
	bool execute (DIV& instr);
	bool execute (ASH& instr);
	bool execute (ASHC& instr);
	bool execute (XOR& instr);
	bool execute (SOB& instr);

	// FIS format instructions
	bool execute (RTS& instr);

	// Branch instructions
	bool execute (BR& instr);
	bool execute (BNE& instr);
	bool execute (BEQ& instr);
	bool execute (BPL& instr);
	bool execute (BMI& instr);
	bool execute (BVC& instr);
	bool execute (BVS& instr);
	bool execute (BCC& instr);
	bool execute (BCS& instr);
	bool execute (BGE& instr);
	bool execute (BLT& instr);
	bool execute (BGT& instr);
	bool execute (BLE& instr);
	bool execute (BHI& instr);
	bool execute (BLOS& instr);
	bool execute (EMT& instr);
	bool execute (TRAP& instr);

	// No-operand instructions
	bool execute (HALT& instr);
	bool execute (WAIT& instr);
	bool execute (RTI& instr);
	bool execute (BPT& instr);
	bool execute (IOT& instr);
	bool execute (RESET& instr);
	bool execute (RTT& instr);
	bool execute (CCC& instr);
	bool execute (SCC& instr);
	bool execute (Unused& instr);

	void setPSW (ConditionCodes conditionCodes);
	constexpr bool isSet (u16 x);

private:
    CpuData* cpuData_;
    CpuControl* cpuControl_;
    MMU* mmu_;

	SingleOperandExecutor singleOperandExecutor_ {cpuData_, cpuControl_, mmu_};
};

constexpr bool Common::Executor::isSet (u16 x)
{
	return (cpuData_->psw () & x) ? true : false;
}

} // namespace Common

#endif // _COMMON_EXECUTOR_H_