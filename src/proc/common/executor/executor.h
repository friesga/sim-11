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
// Roughly two differences exist between the processors:
// 1. The order in which the condition codes and the operands are written
//    back to memory after execution of an instruction.
// 2. The use of the source operand register in certain instructions using the
//    autoincrement or autodecrement addressing mode. For certain processors,
//    such as the KD11-NA (LSI-11), the initial contents of the register are
//    used as the source operand while for other processors, such as the
//    KDF11, the contents of the register are incremented (or decremented)
//    by 2 before being used as the source operand. This results in a
//    different result in the execution of instructions like OPR R,(R)+ and
//    OPR R,-(R) using the same register as both source and destination.
//    This applies to the MOV(B), CMP(B), ADD, SUB, BIT(B), BIC(B) and BIS(B)
//    instructions.
//
// The first difference is implemented in this Executor class by using a
// template parameter WriteOperandOrder which determines the order of writing
// the operands and setting the condition codes. The second difference is
// covered by the same template parameter as these differences probably
// discriminate two types of processor implemenations. If this assumption
// turns out to be incorrect, an additional template parameter needs to be
// added.
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
	template <WriteOperandOrder order>
    bool execute (CLR& instr);
	template <WriteOperandOrder order>
	bool execute (CLRB& instr);

	template <WriteOperandOrder order>
	bool execute (COM& instr);
	template <WriteOperandOrder order>
	bool execute (COMB& instr);

	template <WriteOperandOrder order>
	bool execute (INC& instr);
	template <WriteOperandOrder order>
	bool execute (INCB& instr);

	template <WriteOperandOrder order>
	bool execute (DEC& instr);
	template <WriteOperandOrder order>
	bool execute (DECB& instr);

	template <WriteOperandOrder order>
	bool execute (NEG& instr);
	template <WriteOperandOrder order>
	bool execute (NEGB& instr);

	bool execute (TST& instr);
	bool execute (TSTB& instr);

	template <WriteOperandOrder order>
	bool execute (ASR& instr);
	template <WriteOperandOrder order>
	bool execute (ASRB& instr);

	template <WriteOperandOrder order>
	bool execute (ASL& instr);
	template <WriteOperandOrder order>
	bool execute (ASLB& instr);

	template <WriteOperandOrder order>
	bool execute (ROR& instr);
	template <WriteOperandOrder order>
	bool execute (RORB& instr);

	template <WriteOperandOrder order>
	bool execute (ROL& instr);
	template <WriteOperandOrder order>
	bool execute (ROLB& instr);

	template <WriteOperandOrder order>
	bool execute (SWAB& instr);

	template <WriteOperandOrder order>
	bool execute (ADC& instr);
	template <WriteOperandOrder order>
	bool execute (ADCB& instr);

	template <WriteOperandOrder order>
	bool execute (SBC& instr);
	template <WriteOperandOrder order>
	bool execute (SBCB& instr);

	template <WriteOperandOrder order>
	bool execute (SXT& instr);

	template <WriteOperandOrder order>
	bool execute (MFPS& instr);

	bool execute (MTPS& instr);
	bool execute (JMP& instr);
	bool execute (MARK& instr);

	// Double operand instructions
	template <WriteOperandOrder order>
	bool execute (MOV& instr);

	template <WriteOperandOrder order>
	bool execute (MOVB& instr);

	template <WriteOperandOrder order>
	bool execute (CMP& instr);

	template <WriteOperandOrder order>
	bool execute (CMPB& instr);

	template <WriteOperandOrder order>
	bool execute (ADD& instr);

	template <WriteOperandOrder order>
	bool execute (SUB& instr);

	template <WriteOperandOrder order>
	bool execute (BIT& instr);
	template <WriteOperandOrder order>
	bool execute (BITB& instr);

	template <WriteOperandOrder order>
	bool execute (BIC& instr);
	template <WriteOperandOrder order>
	bool execute (BICB& instr);

	template <WriteOperandOrder order>
	bool execute (BIS& instr);
	template <WriteOperandOrder order>
	bool execute (BISB& instr);

	// EIS instructions, including JSR and XOR
	bool execute (JSR& instr);
	bool execute (MUL& instr);
	bool execute (DIV& instr);
	bool execute (ASH& instr);
	bool execute (ASHC& instr);

	template <WriteOperandOrder order>
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
	
	template <WriteOperandOrder order>
	bool finishExecution (auto&& writeOperands, auto&& setCC);
};

constexpr bool Common::Executor::isSet (u16 x)
{
	return (cpuData_->psw () & x) ? true : false;
}

} // namespace Common


#endif // _COMMON_EXECUTOR_H_