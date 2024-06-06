#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_

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

#include <variant>

using std::variant;

using Instruction = variant<
    CommonInstruction::CLR,
    CommonInstruction::CLRB,
    CommonInstruction::COM,
    CommonInstruction::COMB,
    CommonInstruction::INC,
    CommonInstruction::INCB,
    CommonInstruction::DEC,
    CommonInstruction::DECB,
    CommonInstruction::NEG,
    CommonInstruction::NEGB,
    CommonInstruction::TST,
    CommonInstruction::TSTB,
    CommonInstruction::ASR,
    CommonInstruction::ASRB,
    CommonInstruction::ASL,
    CommonInstruction::ASLB,
    CommonInstruction::ROR,
    CommonInstruction::RORB,
    CommonInstruction::ROL,
    CommonInstruction::ROLB,
    CommonInstruction::SWAB,
    CommonInstruction::ADC,
    CommonInstruction::ADCB,
    CommonInstruction::SBC,
    CommonInstruction::SBCB,
    CommonInstruction::SXT,
    CommonInstruction::MFPS,
    CommonInstruction::MTPS,
    CommonInstruction::JMP,
    CommonInstruction::MARK,
    CommonInstruction::MOV,
    CommonInstruction::MOVB,
    CommonInstruction::CMP,
    CommonInstruction::CMPB,
    CommonInstruction::ADD,
    CommonInstruction::SUB,
    CommonInstruction::BIT,
    CommonInstruction::BITB,
    CommonInstruction::BIC,
    CommonInstruction::BICB,
    CommonInstruction::BIS,
    CommonInstruction::BISB,
    CommonInstruction::JSR,
    CommonInstruction::MUL,
    CommonInstruction::DIV,
    CommonInstruction::ASH,
    CommonInstruction::ASHC,
    CommonInstruction::XOR,
    CommonInstruction::SOB,
    CommonInstruction::RTS,
    CommonInstruction::FADD,
    CommonInstruction::FSUB,
    CommonInstruction::FMUL,
    CommonInstruction::FDIV,
    CommonInstruction::BR,
    CommonInstruction::BNE,
    CommonInstruction::BEQ,
    CommonInstruction::BPL,
    CommonInstruction::BMI,
    CommonInstruction::BVC,
    CommonInstruction::BVS,
    CommonInstruction::BCC,
    CommonInstruction::BCS,
    CommonInstruction::BGE,
    CommonInstruction::BLT,
    CommonInstruction::BGT,
    CommonInstruction::BLE,
    CommonInstruction::BHI,
    CommonInstruction::BLOS,
    CommonInstruction::EMT,
    CommonInstruction::TRAP,
    CommonInstruction::HALT,
    CommonInstruction::WAIT,
    CommonInstruction::RTI,
    CommonInstruction::BPT,
    CommonInstruction::IOT,
    CommonInstruction::RESET,
    CommonInstruction::RTT,
    CommonInstruction::CCC,
    CommonInstruction::SCC, 
    // CommonInstruction::MFPD,
    // CommonInstruction::MFPI,
    // CommonInstruction::MTPD,
    // CommonInstruction::MTPI,
    // CommonInstruction::MFPT,
    CommonInstruction::Unused>;

#endif // _INSTRUCTIONS_H_