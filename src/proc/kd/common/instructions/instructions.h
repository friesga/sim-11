#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_

#include "proc/kd/common/instructions/adc.h"
#include "proc/kd/common/instructions/adcb.h"
#include "proc/kd/common/instructions/add.h"
#include "proc/kd/common/instructions/ash.h"
#include "proc/kd/common/instructions/ashc.h"
#include "proc/kd/common/instructions/asl.h"
#include "proc/kd/common/instructions/aslb.h"
#include "proc/kd/common/instructions/asr.h"
#include "proc/kd/common/instructions/asrb.h"
#include "proc/kd/common/instructions/bcc.h"
#include "proc/kd/common/instructions/bcs.h"
#include "proc/kd/common/instructions/beq.h"
#include "proc/kd/common/instructions/bge.h"
#include "proc/kd/common/instructions/bgt.h"
#include "proc/kd/common/instructions/bhi.h"
#include "proc/kd/common/instructions/bic.h"
#include "proc/kd/common/instructions/bicb.h"
#include "proc/kd/common/instructions/bis.h"
#include "proc/kd/common/instructions/bisb.h"
#include "proc/kd/common/instructions/bit.h"
#include "proc/kd/common/instructions/bitb.h"
#include "proc/kd/common/instructions/ble.h"
#include "proc/kd/common/instructions/blos.h"
#include "proc/kd/common/instructions/blt.h"
#include "proc/kd/common/instructions/bmi.h"
#include "proc/kd/common/instructions/bne.h"
#include "proc/kd/common/instructions/bpl.h"
#include "proc/kd/common/instructions/bpt.h"
#include "proc/kd/common/instructions/br.h"
#include "proc/kd/common/instructions/bvc.h"
#include "proc/kd/common/instructions/bvs.h"
#include "proc/kd/common/instructions/ccc.h"
#include "proc/kd/common/instructions/clr.h"
#include "proc/kd/common/instructions/clrb.h"
#include "proc/kd/common/instructions/cmp.h"
#include "proc/kd/common/instructions/cmpb.h"
#include "proc/kd/common/instructions/com.h"
#include "proc/kd/common/instructions/comb.h"
#include "proc/kd/common/instructions/dec.h"
#include "proc/kd/common/instructions/decb.h"
#include "proc/kd/common/instructions/div.h"
#include "proc/kd/common/instructions/emt.h"
#include "proc/kd/common/instructions/halt.h"
#include "proc/kd/common/instructions/inc.h"
#include "proc/kd/common/instructions/incb.h"
#include "proc/kd/common/instructions/iot.h"
#include "proc/kd/common/instructions/jmp.h"
#include "proc/kd/common/instructions/jsr.h"
#include "proc/kd/common/instructions/mark.h"
#include "proc/kd/common/instructions/mfps.h"
#include "proc/kd/common/instructions/mov.h"
#include "proc/kd/common/instructions/movb.h"
#include "proc/kd/common/instructions/mtps.h"
#include "proc/kd/common/instructions/mul.h"
#include "proc/kd/common/instructions/neg.h"
#include "proc/kd/common/instructions/negb.h"
#include "proc/kd/common/instructions/reset.h"
#include "proc/kd/common/instructions/rol.h"
#include "proc/kd/common/instructions/rolb.h"
#include "proc/kd/common/instructions/ror.h"
#include "proc/kd/common/instructions/rorb.h"
#include "proc/kd/common/instructions/rti.h"
#include "proc/kd/common/instructions/rts.h"
#include "proc/kd/common/instructions/rtt.h"
#include "proc/kd/common/instructions/sbc.h"
#include "proc/kd/common/instructions/sbcb.h"
#include "proc/kd/common/instructions/scc.h"
#include "proc/kd/common/instructions/sob.h"
#include "proc/kd/common/instructions/sub.h"
#include "proc/kd/common/instructions/swab.h"
#include "proc/kd/common/instructions/sxt.h"
#include "proc/kd/common/instructions/trap.h"
#include "proc/kd/common/instructions/tst.h"
#include "proc/kd/common/instructions/tstb.h"
#include "proc/kd/common/instructions/unused.h"
#include "proc/kd/common/instructions/wait.h"
#include "proc/kd/common/instructions/xor.h"

// KD11-NA specific instructions
#include "proc/kd/common/instructions/fadd.h"
#include "proc/kd/common/instructions/fsub.h"
#include "proc/kd/common/instructions/fmul.h"
#include "proc/kd/common/instructions/fdiv.h"

// KDF11-A specific instructions
#include "proc/kd/common/instructions/mfpd.h"
#include "proc/kd/common/instructions/mtpd.h"
#include "proc/kd/common/instructions/mfpi.h"
#include "proc/kd/common/instructions/mtpi.h"
#include "proc/kd/common/instructions/mfpt.h"


#include <variant>

using std::variant;

using Instruction = variant<
    CLR,
    CLRB,
    COM,
    COMB,
    INC,
    INCB,
    DEC,
    DECB,
    NEG,
    NEGB,
    TST,
    TSTB,
    ASR,
    ASRB,
    ASL,
    ASLB,
    ROR,
    RORB,
    ROL,
    ROLB,
    SWAB,
    ADC,
    ADCB,
    SBC,
    SBCB,
    SXT,
    MFPS,
    MTPS,
    JMP,
    MARK,
    MOV,
    MOVB,
    CMP,
    CMPB,
    ADD,
    SUB,
    BIT,
    BITB,
    BIC,
    BICB,
    BIS,
    BISB,
    JSR,
    MUL,
    DIV,
    ASH,
    ASHC,
    XOR,
    SOB,
    RTS,
    FADD,
    FSUB,
    FMUL,
    FDIV,
    BR,
    BNE,
    BEQ,
    BPL,
    BMI,
    BVC,
    BVS,
    BCC,
    BCS,
    BGE,
    BLT,
    BGT,
    BLE,
    BHI,
    BLOS,
    EMT,
    TRAP,
    HALT,
    WAIT,
    RTI,
    BPT,
    IOT,
    RESET,
    RTT,
    CCC,
    SCC,
    MFPD,
    MTPD,
    MFPI,
    MTPI,
    MFPT,
    Unused>;

enum class WriteOperandOrder
{
    WriteOperandBeforeCC,
    WriteOperandAfterCC
};

#endif // _INSTRUCTIONS_H_