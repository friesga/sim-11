#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_

#include "proc/common/instructions/adc.h"
#include "proc/common/instructions/adcb.h"
#include "proc/common/instructions/add.h"
#include "proc/common/instructions/ash.h"
#include "proc/common/instructions/ashc.h"
#include "proc/common/instructions/asl.h"
#include "proc/common/instructions/aslb.h"
#include "proc/common/instructions/asr.h"
#include "proc/common/instructions/asrb.h"
#include "proc/common/instructions/bcc.h"
#include "proc/common/instructions/bcs.h"
#include "proc/common/instructions/beq.h"
#include "proc/common/instructions/bge.h"
#include "proc/common/instructions/bgt.h"
#include "proc/common/instructions/bhi.h"
#include "proc/common/instructions/bic.h"
#include "proc/common/instructions/bicb.h"
#include "proc/common/instructions/bis.h"
#include "proc/common/instructions/bisb.h"
#include "proc/common/instructions/bit.h"
#include "proc/common/instructions/bitb.h"
#include "proc/common/instructions/ble.h"
#include "proc/common/instructions/blos.h"
#include "proc/common/instructions/blt.h"
#include "proc/common/instructions/bmi.h"
#include "proc/common/instructions/bne.h"
#include "proc/common/instructions/bpl.h"
#include "proc/common/instructions/bpt.h"
#include "proc/common/instructions/br.h"
#include "proc/common/instructions/bvc.h"
#include "proc/common/instructions/bvs.h"
#include "proc/common/instructions/ccc.h"
#include "proc/common/instructions/clr.h"
#include "proc/common/instructions/clrb.h"
#include "proc/common/instructions/cmp.h"
#include "proc/common/instructions/cmpb.h"
#include "proc/common/instructions/com.h"
#include "proc/common/instructions/comb.h"
#include "proc/common/instructions/dec.h"
#include "proc/common/instructions/decb.h"
#include "proc/common/instructions/div.h"
#include "proc/common/instructions/emt.h"
#include "proc/common/instructions/halt.h"
#include "proc/common/instructions/inc.h"
#include "proc/common/instructions/incb.h"
#include "proc/common/instructions/iot.h"
#include "proc/common/instructions/jmp.h"
#include "proc/common/instructions/jsr.h"
#include "proc/common/instructions/mark.h"
#include "proc/common/instructions/mfps.h"
#include "proc/common/instructions/mov.h"
#include "proc/common/instructions/movb.h"
#include "proc/common/instructions/mtps.h"
#include "proc/common/instructions/mul.h"
#include "proc/common/instructions/neg.h"
#include "proc/common/instructions/negb.h"
#include "proc/common/instructions/reset.h"
#include "proc/common/instructions/rol.h"
#include "proc/common/instructions/rolb.h"
#include "proc/common/instructions/ror.h"
#include "proc/common/instructions/rorb.h"
#include "proc/common/instructions/rti.h"
#include "proc/common/instructions/rts.h"
#include "proc/common/instructions/rtt.h"
#include "proc/common/instructions/sbc.h"
#include "proc/common/instructions/sbcb.h"
#include "proc/common/instructions/scc.h"
#include "proc/common/instructions/sob.h"
#include "proc/common/instructions/sub.h"
#include "proc/common/instructions/swab.h"
#include "proc/common/instructions/sxt.h"
#include "proc/common/instructions/trap.h"
#include "proc/common/instructions/tst.h"
#include "proc/common/instructions/tstb.h"
#include "proc/common/instructions/unused.h"
#include "proc/common/instructions/wait.h"
#include "proc/common/instructions/xor.h"

// KD11-NA specific instructions
#include "proc/common/instructions/fadd.h"
#include "proc/common/instructions/fsub.h"
#include "proc/common/instructions/fmul.h"
#include "proc/common/instructions/fdiv.h"

// KDF11-A specific instructions
#include "proc/common/instructions/mfpd.h"
#include "proc/common/instructions/mtpd.h"
#include "proc/common/instructions/mfpi.h"
#include "proc/common/instructions/mtpi.h"
#include "proc/common/instructions/mfpt.h"


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