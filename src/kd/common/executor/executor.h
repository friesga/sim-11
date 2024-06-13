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
	// bool operator() (FADD& instr);
	// bool operator() (FSUB& instr);
	// bool operator() (FMUL& instr);
	// bool operator() (FDIV& instr);

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

private:
    CpuData* cpuData_;
    CpuControl* cpuControl_;
    MMU* mmu_;

    void setPSW (ConditionCodes conditionCodes);
	constexpr bool isSet (u16 x);
};

} // namespace Common

#endif // _COMMON_EXECUTOR_H_