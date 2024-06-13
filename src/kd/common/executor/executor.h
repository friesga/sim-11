#ifndef _COMMON_EXECUTOR_H_
#define _COMMON_EXECUTOR_H_

#include "kd/common/instructions/instructions.h"

/*
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/include/mmu.h"

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

// LSI-11 specific instructions
#include "kd/common/instructions/fadd.h"
#include "kd/common/instructions/fsub.h"
#include "kd/common/instructions/fmul.h"
#include "kd/common/instructions/fdiv.h"

// KDF11-A specific instructions
#include "kd/common/instructions/mfpd.h"
#include "kd/common/instructions/mtpd.h"
#include "kd/common/instructions/mfpt.h"
*/

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
	// bool execute (FADD& instr);
	// bool execute (FSUB& instr);
	// bool execute (FMUL& instr);
	// bool execute (FDIV& instr);

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

private:
    CpuData* cpuData_;
    CpuControl* cpuControl_;
    MMU* mmu_;

    void setPSW (ConditionCodes conditionCodes);
	constexpr bool isSet (u16 x);
};

} // namespace Common

#endif // _COMMON_EXECUTOR_H_