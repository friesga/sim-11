#ifndef _COMMON_EXECUTOR_H_
#define _COMMON_EXECUTOR_H_

#include "proc/kd/common/instructions/instructions.h"
#include "proc/kd/common/operanddecoderfactory/operanddecoderfactory.h"

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
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
    bool execute (CLR& instr);
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (CLRB& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (COM& instr);
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (COMB& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (INC& instr);
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (INCB& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (DEC& instr);
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (DECB& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (NEG& instr);
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (NEGB& instr);

	bool execute (TST& instr);
	bool execute (TSTB& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (ASR& instr);
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (ASRB& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (ASL& instr);
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (ASLB& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (ROR& instr);
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (RORB& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (ROL& instr);
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (ROLB& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (SWAB& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (ADC& instr);
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (ADCB& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (SBC& instr);
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (SBCB& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (SXT& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (MFPS& instr);

	bool execute (MTPS& instr);
	bool execute (JMP& instr);
	bool execute (MARK& instr);

	// Double operand instructions
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (MOV& instr);
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (MOVB& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (CMP& instr);
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (CMPB& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (ADD& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (SUB& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (BIT& instr);
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (BITB& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (BIC& instr);
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (BICB& instr);

	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
	bool execute (BIS& instr);
	template <WriteOperandOrder order = WriteOperandOrder::WriteOperandBeforeCC>
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

	OperandDecoderFactory operandDecoderFactory_ {cpuData_, cpuControl_, mmu_};
};

constexpr bool Common::Executor::isSet (u16 x)
{
	return (cpuData_->psw () & x) ? true : false;
}

} // namespace Common

#endif // _COMMON_EXECUTOR_H_