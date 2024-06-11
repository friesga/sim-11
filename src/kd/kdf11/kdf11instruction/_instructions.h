#ifndef _KDF11_A_INSTRUCTIONS_H_
#define _KDF11_A_INSTRUCTIONS_H_

#include "adc.h"
#include "adcb.h"
#include "add.h"
#include "kd/common/instructions/ash.h"
#include "kd/common/instructions/ashc.h"
#include "asl.h"
#include "aslb.h"
#include "asr.h"
#include "asrb.h"
#include "kd/common/instructions/bcc.h"
#include "kd/common/instructions/bcs.h"
#include "kd/common/instructions/beq.h"
#include "kd/common/instructions/bge.h"
#include "kd/common/instructions/bgt.h"
#include "kd/common/instructions/bhi.h"
#include "bic.h"
#include "bicb.h"
#include "bis.h"
#include "bisb.h"
#include "bit.h"
#include "bitb.h"
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
#include "clr.h"
#include "clrb.h"
#include "cmp.h"
#include "cmpb.h"
#include "com.h"
#include "comb.h"
#include "dec.h"
#include "decb.h"
#include "kd/common/instructions/div.h"
#include "kd/common/instructions/emt.h"
#include "halt.h"
#include "inc.h"
#include "incb.h"
#include "kd/common/instructions/iot.h"
#include "kd/common/instructions/jmp.h"
#include "kd/common/instructions/jsr.h"
#include "kd/common/instructions/mark.h"
#include "mfpd.h"
#include "mfps.h"
#include "mfpt.h"
#include "mtpd.h"
#include "mov.h"
#include "movb.h"
#include "kd/common/instructions/mtps.h"
#include "kd/common/instructions/mul.h"
#include "neg.h"
#include "negb.h"
#include "reset.h"
#include "rol.h"
#include "rolb.h"
#include "ror.h"
#include "rorb.h"
#include "kd/common/instructions/rti.h"
#include "kd/common/instructions/rts.h"
#include "kd/common/instructions/rtt.h"
#include "sbc.h"
#include "sbcb.h"
#include "kd/common/instructions/scc.h"
#include "kd/common/instructions/sob.h"
#include "sub.h"
#include "swab.h"
#include "sxt.h"
#include "kd/common/instructions/trap.h"
#include "kd/common/instructions/tst.h"
#include "kd/common/instructions/tstb.h"
#include "kd/common/instructions/unused.h"
#include "wait.h"
#include "xor.h"

#include <variant>

using std::variant;

using Instruction = variant<
    KDF11Instruction::CLR,
    KDF11Instruction::CLRB,
    KDF11Instruction::COM,
    KDF11Instruction::COMB,
    KDF11Instruction::INC,
    KDF11Instruction::INCB,
    KDF11Instruction::DEC,
    KDF11Instruction::DECB,
    KDF11Instruction::NEG,
    KDF11Instruction::NEGB,
    TST,
    TSTB,
    KDF11Instruction::ASR,
    KDF11Instruction::ASRB,
    KDF11Instruction::ASL,
    KDF11Instruction::ASLB,
    KDF11Instruction::ROR,
    KDF11Instruction::RORB,
    KDF11Instruction::ROL,
    KDF11Instruction::ROLB,
    KDF11Instruction::SWAB,
    KDF11Instruction::ADC,
    KDF11Instruction::ADCB,
    KDF11Instruction::SBC,
    KDF11Instruction::SBCB,
    KDF11Instruction::SXT,
    KDF11Instruction::MFPS,
    MTPS,
    JMP,
    MARK,
    KDF11Instruction::MOV,
    KDF11Instruction::MOVB,
    KDF11Instruction::CMP,
    KDF11Instruction::CMPB,
    KDF11Instruction::ADD,
    KDF11Instruction::SUB,
    KDF11Instruction::BIT,
    KDF11Instruction::BITB,
    KDF11Instruction::BIC,
    KDF11Instruction::BICB,
    KDF11Instruction::BIS,
    KDF11Instruction::BISB,
    JSR,
    MUL,
    DIV,
    ASH,
    ASHC,
    KDF11Instruction::XOR,
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
    KDF11Instruction::HALT,
    KDF11Instruction::WAIT,
    RTI,
    BPT,
    IOT,
    KDF11Instruction::RESET,
    RTT,
    CCC,
    SCC, 
    KDF11Instruction::MFPD,
    KDF11Instruction::MTPD,
    KDF11Instruction::MFPT,
    Unused>;

#endif // _KDF11_A_INSTRUCTIONS_H_