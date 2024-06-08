#ifndef _EXECUTOR_H_
#define _EXECUTOR_H_

#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/include/mmu.h"

#include "kd/common/instructions/commoninstruction.h"

#include "kd/common/instructions/adc.h"
#include "kd/common/instructions/adcb.h"
#include "kd/common/instructions/add.h"
#include "kd/common/instructions/ash.h"
#include "kd/common/instructions/ashc.h"
#include "kd/common/instructions/asl.h"
#include "kd/common/instructions/aslb.h"
#include "kd/common/instructions/asr.h"
#include "kd/common/instructions/asrb.h"
#include "kd/common/instructions/bcc.h"
#include "kd/common/instructions/bcs.h"
#include "kd/common/instructions/beq.h"
#include "kd/common/instructions/bge.h"
#include "kd/common/instructions/bgt.h"
#include "kd/common/instructions/bhi.h"
#include "kd/common/instructions/bic.h"
#include "kd/common/instructions/bicb.h"
#include "kd/common/instructions/bis.h"
#include "kd/common/instructions/bisb.h"
#include "kd/common/instructions/bit.h"
#include "kd/common/instructions/bitb.h"
#include "kd/common/instructions/ble.h"
#include "kd/common/instructions/blos.h"
#include "kd/common/instructions/blt.h"
#include "kd/common/instructions/bmi.h"
#include "kd/common/instructions/bne.h"
#include "kd/common/instructions/bpl.h"
#include "kd/common/instructions/bpt.h"
#include "kd/common/instructions/br.h"
#include "kd/common/instructions/bvc.h"
#include "kd/common/instructions/bvs.h"
#include "kd/common/instructions/ccc.h"
#include "kd/common/instructions/clr.h"
#include "kd/common/instructions/clrb.h"
#include "kd/common/instructions/cmp.h"
#include "kd/common/instructions/cmpb.h"
#include "kd/common/instructions/com.h"
#include "kd/common/instructions/comb.h"
#include "kd/common/instructions/dec.h"
#include "kd/common/instructions/decb.h"
#include "kd/common/instructions/div.h"
#include "kd/common/instructions/emt.h"
#include "kd/common/instructions/fadd.h"
#include "kd/common/instructions/fdiv.h"
#include "kd/common/instructions/fmul.h"
#include "kd/common/instructions/fsub.h"
#include "kd/common/instructions/halt.h"
#include "kd/common/instructions/inc.h"
#include "kd/common/instructions/incb.h"
#include "kd/common/instructions/iot.h"
#include "kd/common/instructions/jmp.h"
#include "kd/common/instructions/jsr.h"
#include "kd/common/instructions/mark.h"
#include "kd/common/instructions/mfps.h"
#include "kd/common/instructions/mov.h"
#include "kd/common/instructions/movb.h"
#include "kd/common/instructions/mtps.h"
#include "kd/common/instructions/mul.h"
#include "kd/common/instructions/neg.h"
#include "kd/common/instructions/negb.h"
#include "kd/common/instructions/reset.h"
#include "kd/common/instructions/rol.h"
#include "kd/common/instructions/rolb.h"
#include "kd/common/instructions/ror.h"
#include "kd/common/instructions/rorb.h"
#include "kd/common/instructions/rti.h"
#include "kd/common/instructions/rts.h"
#include "kd/common/instructions/rtt.h"
#include "kd/common/instructions/sbc.h"
#include "kd/common/instructions/sbcb.h"
#include "kd/common/instructions/scc.h"
#include "kd/common/instructions/sob.h"
#include "kd/common/instructions/sub.h"
#include "kd/common/instructions/swab.h"
#include "kd/common/instructions/sxt.h"
#include "kd/common/instructions/trap.h"
#include "kd/common/instructions/tst.h"
#include "kd/common/instructions/tstb.h"
#include "kd/common/instructions/unused.h"
#include "kd/common/instructions/wait.h"
#include "kd/common/instructions/xor.h"

namespace KD11_NA {

// This class executes KD11-NA instructions
//
// ToDo: Change this class to function object with a per instruction overloaded
// operator() function.
//
class Executor
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

private:
    CpuData* cpuData_;
    CpuControl* cpuControl_;
    MMU* mmu_;

    void setPSW (ConditionCodes conditionCodes);
	constexpr bool isSet (u16 x);
};

} // namespace KD11_NA

#endif // _EXECUTOR_H_