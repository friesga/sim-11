#include "kdf11instruction.h"
#include "kd/common/instructions/commoninstruction.h"

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

// Instruction format for Operate Group plus BPT and IOT,
//
// and minus NOP instructions.
KDF11Instruction::opCodeTable const KDF11Instruction::group_00_00_nn
{ 
    create<KDF11Instruction::HALT>,        // 00 00 00 
    create<KDF11Instruction::WAIT>,        // 00 00 01
    create<CommonInstruction::RTI>,        // 00 00 02
    create<CommonInstruction::BPT>,        // 00 00 03
    create<CommonInstruction::IOT>,        // 00 00 04
    create<KDF11Instruction::RESET>,       // 00 00 05
    create<CommonInstruction::RTT>,        // 00 00 06
    create<KDF11Instruction::MFPT>         // 00 00 07
};

// Instruction format for RTS and Condition Code Operators instructions
KDF11Instruction::opCodeTable const KDF11Instruction::group_00_02_nn
{
    create<CommonInstruction::RTS>,        // 00 02 00
    create<CommonInstruction::RTS>,        // 00 02 01
    create<CommonInstruction::RTS>,        // 00 02 02
    create<CommonInstruction::RTS>,        // 00 02 03
    create<CommonInstruction::RTS>,        // 00 02 04
    create<CommonInstruction::RTS>,        // 00 02 05
    create<CommonInstruction::RTS>,        // 00 02 06
    create<CommonInstruction::RTS>,        // 00 02 07
    create<CommonInstruction::Unused>,     // 00 02 10
    create<CommonInstruction::Unused>,     // 00 02 11
    create<CommonInstruction::Unused>,     // 00 02 12
    create<CommonInstruction::Unused>,     // 00 02 13
    create<CommonInstruction::Unused>,     // 00 02 14
    create<CommonInstruction::Unused>,     // 00 02 15
    create<CommonInstruction::Unused>,     // 00 02 16
    create<CommonInstruction::Unused>,     // 00 02 17
    create<CommonInstruction::Unused>,     // 00 02 20
    create<CommonInstruction::Unused>,     // 00 02 21
    create<CommonInstruction::Unused>,     // 00 02 22
    create<CommonInstruction::Unused>,     // 00 02 23
    create<CommonInstruction::Unused>,     // 00 02 24
    create<CommonInstruction::Unused>,     // 00 02 25
    create<CommonInstruction::Unused>,     // 00 02 26
    create<CommonInstruction::Unused>,     // 00 02 27
    create<CommonInstruction::Unused>,     // 00 02 30
    create<CommonInstruction::Unused>,     // 00 02 31
    create<CommonInstruction::Unused>,     // 00 02 32
    create<CommonInstruction::Unused>,     // 00 02 33
    create<CommonInstruction::Unused>,     // 00 02 34
    create<CommonInstruction::Unused>,     // 00 02 35
    create<CommonInstruction::Unused>,     // 00 02 36
    create<CommonInstruction::Unused>,     // 00 02 37
    create<CommonInstruction::CCC>,        // 00 02 40
    create<CommonInstruction::CCC>,        // 00 02 41
    create<CommonInstruction::CCC>,        // 00 02 42
    create<CommonInstruction::CCC>,        // 00 02 43
    create<CommonInstruction::CCC>,        // 00 02 44
    create<CommonInstruction::CCC>,        // 00 02 45
    create<CommonInstruction::CCC>,        // 00 02 46
    create<CommonInstruction::CCC>,        // 00 02 47
    create<CommonInstruction::CCC>,        // 00 02 50
    create<CommonInstruction::CCC>,        // 00 02 51
    create<CommonInstruction::CCC>,        // 00 02 52
    create<CommonInstruction::CCC>,        // 00 02 53
    create<CommonInstruction::CCC>,        // 00 02 54
    create<CommonInstruction::CCC>,        // 00 02 55
    create<CommonInstruction::CCC>,        // 00 02 56
    create<CommonInstruction::CCC>,        // 00 02 57
    create<CommonInstruction::SCC>,        // 00 02 60
    create<CommonInstruction::SCC>,        // 00 02 61
    create<CommonInstruction::SCC>,        // 00 02 62
    create<CommonInstruction::SCC>,        // 00 02 63
    create<CommonInstruction::SCC>,        // 00 02 64
    create<CommonInstruction::SCC>,        // 00 02 65
    create<CommonInstruction::SCC>,        // 00 02 66
    create<CommonInstruction::SCC>,        // 00 02 67
    create<CommonInstruction::SCC>,        // 00 02 70
    create<CommonInstruction::SCC>,        // 00 02 71
    create<CommonInstruction::SCC>,        // 00 02 72
    create<CommonInstruction::SCC>,        // 00 02 73
    create<CommonInstruction::SCC>,        // 00 02 74
    create<CommonInstruction::SCC>,        // 00 02 75
    create<CommonInstruction::SCC>,        // 00 02 76
    create<CommonInstruction::SCC>         // 00 02 77
};

// Instruction format for Fixed and Floating Point Arithmetic instructions,
// plus the SOB and XOR instructions.
// 
// The FIS instructions are defined in a separate group.
//
KDF11Instruction::opCodeTable const KDF11Instruction::group_07_nx_xx
{
    create<CommonInstruction::MUL>,        // 07 0x xx
    create<CommonInstruction::DIV>,        // 07 1x xx
    create<CommonInstruction::ASH>,        // 07 2x xx
    create<CommonInstruction::ASHC>,       // 07 3x xx
    create<KDF11Instruction::XOR>,         // 07 4x xx
    create<CommonInstruction::Unused>,     // 07 5x xx
    create<CommonInstruction::Unused>,     // 07 6x xx
    create<CommonInstruction::SOB>         // 07 7x xx
};

// Instruction format for the Program Control and Single Operand Group
// instructions.
KDF11Instruction::opCodeTable const KDF11Instruction::group_10_xx_xx
{
    create<CommonInstruction::BPL>,    // 10 00 xx
    create<CommonInstruction::BPL>,    // 10 01 xx
    create<CommonInstruction::BPL>,    // 10 02 xx
    create<CommonInstruction::BPL>,    // 10 03 xx
    create<CommonInstruction::BMI>,    // 10 04 xx
    create<CommonInstruction::BMI>,    // 10 05 xx
    create<CommonInstruction::BMI>,    // 10 06 xx
    create<CommonInstruction::BMI>,    // 10 07 xx
    create<CommonInstruction::BHI>,    // 10 10 xx
    create<CommonInstruction::BHI>,    // 10 11 xx
    create<CommonInstruction::BHI>,    // 10 12 xx
    create<CommonInstruction::BHI>,    // 10 13 xx
    create<CommonInstruction::BLOS>,   // 10 14 xx
    create<CommonInstruction::BLOS>,   // 10 15 xx
    create<CommonInstruction::BLOS>,   // 10 16 xx
    create<CommonInstruction::BLOS>,   // 10 17 xx
    create<CommonInstruction::BVC>,    // 10 20 xx
    create<CommonInstruction::BVC>,    // 10 21 xx
    create<CommonInstruction::BVC>,    // 10 22 xx
    create<CommonInstruction::BVC>,    // 10 23 xx
    create<CommonInstruction::BVS>,    // 10 24 xx
    create<CommonInstruction::BVS>,    // 10 25 xx
    create<CommonInstruction::BVS>,    // 10 26 xx
    create<CommonInstruction::BVS>,    // 10 27 xx
    create<CommonInstruction::BCC>,    // 10 30 xx - BHIS is the same instruction as BCC
    create<CommonInstruction::BCC>,    // 10 31 xx
    create<CommonInstruction::BCC>,    // 10 32 xx
    create<CommonInstruction::BCC>,    // 10 33 xx
    create<CommonInstruction::BCS>,    // 10 34 xx - BLO is the same instruction as BCS
    create<CommonInstruction::BCS>,    // 10 35 xx
    create<CommonInstruction::BCS>,    // 10 36 xx
    create<CommonInstruction::BCS>,    // 10 37 xx
    create<CommonInstruction::EMT>,    // 10 40 xx
    create<CommonInstruction::EMT>,    // 10 41 xx
    create<CommonInstruction::EMT>,    // 10 42 xx
    create<CommonInstruction::EMT>,    // 10 43 xx
    create<CommonInstruction::TRAP>,   // 10 44 xx
    create<CommonInstruction::TRAP>,   // 10 45 xx
    create<CommonInstruction::TRAP>,   // 10 46 xx
    create<CommonInstruction::TRAP>,   // 10 47 xx
    create<KDF11Instruction::CLRB>,    // 10 50 xx
    create<KDF11Instruction::COMB>,    // 10 51 xx
    create<KDF11Instruction::INCB>,    // 10 52 xx
    create<KDF11Instruction::DECB>,    // 10 53 xx
    create<KDF11Instruction::NEGB>,    // 10 54 xx
    create<KDF11Instruction::ADCB>,    // 10 55 xx
    create<KDF11Instruction::SBCB>,    // 10 56 xx
    create<CommonInstruction::TSTB>,   // 10 57 xx
    create<KDF11Instruction::RORB>,    // 10 60 xx
    create<KDF11Instruction::ROLB>,    // 10 61 xx
    create<KDF11Instruction::ASRB>,    // 10 62 xx
    create<KDF11Instruction::ASLB>,    // 10 63 xx
    create<CommonInstruction::MTPS>,   // 10 64 xx
    create<KDF11Instruction::MFPD>,    // 10 65 xx MFPD and MFPI are identical instructions
    create<KDF11Instruction::MTPD>,    // 10 66 xx MTPD and MTPI are identical instructions
    create<KDF11Instruction::MFPS>,    // 10 67 xx
    create<CommonInstruction::Unused>, // 10 70 xx The range 10 70 xx till 10 77 xx is undefined
    create<CommonInstruction::Unused>, // 10 71 xx in the LSI-11 Processor Handbook
    create<CommonInstruction::Unused>, // 10 72 xx
    create<CommonInstruction::Unused>, // 10 73 xx
    create<CommonInstruction::Unused>, // 10 74 xx
    create<CommonInstruction::Unused>, // 10 75 xx
    create<CommonInstruction::Unused>, // 10 76 xx
    create<CommonInstruction::Unused>, // 10 77 xx
};

// Instruction format for the Program Control and Single Operand Group
// instructions. 
KDF11Instruction::opCodeTable const KDF11Instruction::group_00_nn_xx
{ 
    decodeGroup_00_00_nn,                              // 00 00 NN
    create<CommonInstruction::JMP>,                    // 00 01 xx
    decodeGroup_00_02_nn,                              // 00 02 NN
    create<KDF11Instruction::SWAB>,                    // 00 03 xx
    create<CommonInstruction::BR>,                     // 00 04 xx
    create<CommonInstruction::BR>,                     // 00 05 xx
    create<CommonInstruction::BR>,                     // 00 06 xx
    create<CommonInstruction::BR>,                     // 00 07 xx
    create<CommonInstruction::BNE>,                    // 00 10 xx
    create<CommonInstruction::BNE>,                    // 00 11 xx
    create<CommonInstruction::BNE>,                    // 00 12 xx
    create<CommonInstruction::BNE>,                    // 00 13 xx
    create<CommonInstruction::BEQ>,                    // 00 14 xx
    create<CommonInstruction::BEQ>,                    // 00 15 xx
    create<CommonInstruction::BEQ>,                    // 00 16 xx
    create<CommonInstruction::BEQ>,                    // 00 17 xx
    create<CommonInstruction::BGE>,                    // 00 20 xx
    create<CommonInstruction::BGE>,                    // 00 21 xx
    create<CommonInstruction::BGE>,                    // 00 22 xx
    create<CommonInstruction::BGE>,                    // 00 23 xx
    create<CommonInstruction::BLT>,                    // 00 24 xx
    create<CommonInstruction::BLT>,                    // 00 25 xx
    create<CommonInstruction::BLT>,                    // 00 26 xx
    create<CommonInstruction::BLT>,                    // 00 27 xx
    create<CommonInstruction::BGT>,                    // 00 30 xx
    create<CommonInstruction::BGT>,                    // 00 31 xx
    create<CommonInstruction::BGT>,                    // 00 32 xx
    create<CommonInstruction::BGT>,                    // 00 33 xx
    create<CommonInstruction::BLE>,                    // 00 34 xx
    create<CommonInstruction::BLE>,                    // 00 35 xx
    create<CommonInstruction::BLE>,                    // 00 36 xx
    create<CommonInstruction::BLE>,                    // 00 37 xx
    create<CommonInstruction::JSR>,                    // 00 40 xx
    create<CommonInstruction::JSR>,                    // 00 41 xx
    create<CommonInstruction::JSR>,                    // 00 42 xx
    create<CommonInstruction::JSR>,                    // 00 43 xx
    create<CommonInstruction::JSR>,                    // 00 44 xx
    create<CommonInstruction::JSR>,                    // 00 45 xx
    create<CommonInstruction::JSR>,                    // 00 46 xx
    create<CommonInstruction::JSR>,                    // 00 47 xx
    create<KDF11Instruction::CLR>,                     // 00 50 xx
    create<KDF11Instruction::COM>,                     // 00 51 xx
    create<KDF11Instruction::INC>,                     // 00 52 xx
    create<KDF11Instruction::DEC>,                     // 00 53 xx
    create<KDF11Instruction::NEG>,                     // 00 54 xx
    create<KDF11Instruction::ADC>,                     // 00 55 xx
    create<KDF11Instruction::SBC>,                     // 00 56 xx
    create<CommonInstruction::TST>,                    // 00 57 xx
    create<KDF11Instruction::ROR>,                     // 00 60 xx
    create<KDF11Instruction::ROL>,                     // 00 61 xx
    create<KDF11Instruction::ASR>,                     // 00 62 xx
    create<KDF11Instruction::ASL>,                     // 00 63 xx
    create<CommonInstruction::MARK>,                   // 00 64 xx
    create<KDF11Instruction::MFPD>,                    // 00 65 xx KDF11-A specific instruction
    create<KDF11Instruction::MTPD>,                    // 00 66 xx idem
    create<KDF11Instruction::SXT>,                     // 00 67 xx
    create<CommonInstruction::Unused>,                 // 00 70 xx
    create<CommonInstruction::Unused>,                 // 00 71 xx
    create<CommonInstruction::Unused>,                 // 00 72 xx
    create<CommonInstruction::Unused>,                 // 00 73 xx
    create<CommonInstruction::Unused>,                 // 00 74 xx
    create<CommonInstruction::Unused>,                 // 00 75 xx
    create<CommonInstruction::Unused>,                 // 00 76 xx
    create<CommonInstruction::Unused>,                 // 00 77 xx
};

// This is the main operation code control table.
KDF11Instruction::opCodeTable const KDF11Instruction::group_nn_xx_xx 
{
    decodeGroup_00_nn_xx,                              // 00 xx xx
    create<KDF11Instruction::MOV>,                     // 01 xx xx
    create<KDF11Instruction::CMP>,                     // 02 xx xx
    create<KDF11Instruction::BIT>,                     // 03 xx xx
    create<KDF11Instruction::BIC>,                     // 04 xx xx
    create<KDF11Instruction::BIS>,                     // 05 xx xx
    create<KDF11Instruction::ADD>,                     // 06 xx xx
    decodeGroup_07_nx_xx,                              // 07 nx xx
    decodeGroup_10_xx_xx,                              // 10 xx xx
    create<KDF11Instruction::MOVB>,                    // 11 xx xx
    create<KDF11Instruction::CMPB>,                    // 12 xx xx
    create<KDF11Instruction::BITB>,                    // 13 xx xx
    create<KDF11Instruction::BICB>,                    // 14 xx xx
    create<KDF11Instruction::BISB>,                    // 15 xx xx
    create<KDF11Instruction::SUB>,                     // 16 xx xx
    create<CommonInstruction::Unused>                  // 17 xx xx Undefined in the LSI-11 Processor Handbook
};