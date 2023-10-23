#include "kdf11_ainstruction.h"
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
#include "clr.h"
#include "clrb.h"
#include "kd/common/instructions/cmp.h"
#include "kd/common/instructions/cmpb.h"
#include "com.h"
#include "comb.h"
#include "dec.h"
#include "decb.h"
#include "kd/common/instructions/div.h"
#include "kd/common/instructions/emt.h"
#include "kd/common/instructions/fadd.h"
#include "kd/common/instructions/fdiv.h"
#include "kd/common/instructions/fmul.h"
#include "kd/common/instructions/fsub.h"
#include "kd/common/instructions/halt.h"
#include "inc.h"
#include "incb.h"
#include "kd/common/instructions/iot.h"
#include "kd/common/instructions/jmp.h"
#include "kd/common/instructions/jsr.h"
#include "kd/common/instructions/mark.h"
#include "mfpd.h"
#include "kd/common/instructions/mfps.h"
#include "mfpt.h"
#include "mtpd.h"
#include "mov.h"
#include "movb.h"
#include "kd/common/instructions/mtps.h"
#include "kd/common/instructions/mul.h"
#include "neg.h"
#include "negb.h"
#include "kd/common/instructions/reset.h"
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
#include "kd/common/instructions/wait.h"
#include "xor.h"

// Instruction format for Operate Group plus BPT and IOT,
//
// and minus NOP instructions.
KDF11_AInstruction::opCodeTable const KDF11_AInstruction::group_00_00_nn
{ 
    CommonInstruction::HALT::create,       // 00 00 00 
    CommonInstruction::WAIT::create,       // 00 00 01
    CommonInstruction::RTI::create,        // 00 00 02
    CommonInstruction::BPT::create,        // 00 00 03
    CommonInstruction::IOT::create,        // 00 00 04
    CommonInstruction::RESET::create,      // 00 00 05
    CommonInstruction::RTT::create,        // 00 00 06
    KDF11_AInstruction::MFPT::create       // 00 00 07
};

// Instruction format for RTS and Condition Code Operators instructions
KDF11_AInstruction::opCodeTable const KDF11_AInstruction::group_00_02_nn
{
    CommonInstruction::RTS::create,        // 00 02 00
    CommonInstruction::RTS::create,        // 00 02 01
    CommonInstruction::RTS::create,        // 00 02 02
    CommonInstruction::RTS::create,        // 00 02 03
    CommonInstruction::RTS::create,        // 00 02 04
    CommonInstruction::RTS::create,        // 00 02 05
    CommonInstruction::RTS::create,        // 00 02 06
    CommonInstruction::RTS::create,        // 00 02 07
    CommonInstruction::Unused::create,     // 00 02 10
    CommonInstruction::Unused::create,     // 00 02 11
    CommonInstruction::Unused::create,     // 00 02 12
    CommonInstruction::Unused::create,     // 00 02 13
    CommonInstruction::Unused::create,     // 00 02 14
    CommonInstruction::Unused::create,     // 00 02 15
    CommonInstruction::Unused::create,     // 00 02 16
    CommonInstruction::Unused::create,     // 00 02 17
    CommonInstruction::Unused::create,     // 00 02 20
    CommonInstruction::Unused::create,     // 00 02 21
    CommonInstruction::Unused::create,     // 00 02 22
    CommonInstruction::Unused::create,     // 00 02 23
    CommonInstruction::Unused::create,     // 00 02 24
    CommonInstruction::Unused::create,     // 00 02 25
    CommonInstruction::Unused::create,     // 00 02 26
    CommonInstruction::Unused::create,     // 00 02 27
    CommonInstruction::Unused::create,     // 00 02 30
    CommonInstruction::Unused::create,     // 00 02 31
    CommonInstruction::Unused::create,     // 00 02 32
    CommonInstruction::Unused::create,     // 00 02 33
    CommonInstruction::Unused::create,     // 00 02 34
    CommonInstruction::Unused::create,     // 00 02 35
    CommonInstruction::Unused::create,     // 00 02 36
    CommonInstruction::Unused::create,     // 00 02 37
    CommonInstruction::CCC::create,        // 00 02 40
    CommonInstruction::CCC::create,        // 00 02 41
    CommonInstruction::CCC::create,        // 00 02 42
    CommonInstruction::CCC::create,        // 00 02 43
    CommonInstruction::CCC::create,        // 00 02 44
    CommonInstruction::CCC::create,        // 00 02 45
    CommonInstruction::CCC::create,        // 00 02 46
    CommonInstruction::CCC::create,        // 00 02 47
    CommonInstruction::CCC::create,        // 00 02 50
    CommonInstruction::CCC::create,        // 00 02 51
    CommonInstruction::CCC::create,        // 00 02 52
    CommonInstruction::CCC::create,        // 00 02 53
    CommonInstruction::CCC::create,        // 00 02 54
    CommonInstruction::CCC::create,        // 00 02 55
    CommonInstruction::CCC::create,        // 00 02 56
    CommonInstruction::CCC::create,        // 00 02 57
    CommonInstruction::SCC::create,        // 00 02 60
    CommonInstruction::SCC::create,        // 00 02 61
    CommonInstruction::SCC::create,        // 00 02 62
    CommonInstruction::SCC::create,        // 00 02 63
    CommonInstruction::SCC::create,        // 00 02 64
    CommonInstruction::SCC::create,        // 00 02 65
    CommonInstruction::SCC::create,        // 00 02 66
    CommonInstruction::SCC::create,        // 00 02 67
    CommonInstruction::SCC::create,        // 00 02 70
    CommonInstruction::SCC::create,        // 00 02 71
    CommonInstruction::SCC::create,        // 00 02 72
    CommonInstruction::SCC::create,        // 00 02 73
    CommonInstruction::SCC::create,        // 00 02 74
    CommonInstruction::SCC::create,        // 00 02 75
    CommonInstruction::SCC::create,        // 00 02 76
    CommonInstruction::SCC::create         // 00 02 77
};

// Instruction format for Fixed and Floating Point Arithmetic instructions
// (just the FIS part of the instruction format).
// 
// Just the operation codes in the range 07 50 0x till 07 50 3x are used.
// The range 07 51 xx till 07 57 xx is handled in the corresponding function
// to minimize the size of this opcode table.
//
KDF11_AInstruction::opCodeTable const KDF11_AInstruction::group_07_5n_nx
{
    CommonInstruction::FADD::create,       // 07 50 0x
    CommonInstruction::FSUB::create,       // 07 50 1x
    CommonInstruction::FMUL::create,       // 07 50 2x
    CommonInstruction::FDIV::create,       // 07 50 3x
    CommonInstruction::Unused::create,     // 07 50 4x
    CommonInstruction::Unused::create,     // 07 50 5x
    CommonInstruction::Unused::create,     // 07 50 6x
    CommonInstruction::Unused::create      // 07 50 7x
};

// Instruction format for Fixed and Floating Point Arithmetic instructions,
// plus the SOB and XOR instructions.
// 
// The FIS instructions are defined in a separate group.
//
KDF11_AInstruction::opCodeTable const KDF11_AInstruction::group_07_nx_xx
{
    CommonInstruction::MUL::create,                    // 07 0x xx
    CommonInstruction::DIV::create,                    // 07 1x xx
    CommonInstruction::ASH::create,                    // 07 2x xx
    CommonInstruction::ASHC::create,                   // 07 3x xx
    KDF11_AInstruction::XOR::create,                   // 07 4x xx
    decodeGroup_07_5n_nx,                              // 07 5x xx
    CommonInstruction::Unused::create,                 // 07 6x xx
    CommonInstruction::SOB::create                     // 07 7x xx
};

// Instruction format for the Program Control and Single Operand Group
// instructions.
KDF11_AInstruction::opCodeTable const KDF11_AInstruction::group_10_xx_xx
{
    CommonInstruction::BPL::create,    // 10 00 xx
    CommonInstruction::BPL::create,    // 10 01 xx
    CommonInstruction::BPL::create,    // 10 02 xx
    CommonInstruction::BPL::create,    // 10 03 xx
    CommonInstruction::BMI::create,    // 10 04 xx
    CommonInstruction::BMI::create,    // 10 05 xx
    CommonInstruction::BMI::create,    // 10 06 xx
    CommonInstruction::BMI::create,    // 10 07 xx
    CommonInstruction::BHI::create,    // 10 10 xx
    CommonInstruction::BHI::create,    // 10 11 xx
    CommonInstruction::BHI::create,    // 10 12 xx
    CommonInstruction::BHI::create,    // 10 13 xx
    CommonInstruction::BLOS::create,   // 10 14 xx
    CommonInstruction::BLOS::create,   // 10 15 xx
    CommonInstruction::BLOS::create,   // 10 16 xx
    CommonInstruction::BLOS::create,   // 10 17 xx
    CommonInstruction::BVC::create,    // 10 20 xx
    CommonInstruction::BVC::create,    // 10 21 xx
    CommonInstruction::BVC::create,    // 10 22 xx
    CommonInstruction::BVC::create,    // 10 23 xx
    CommonInstruction::BVS::create,    // 10 24 xx
    CommonInstruction::BVS::create,    // 10 25 xx
    CommonInstruction::BVS::create,    // 10 26 xx
    CommonInstruction::BVS::create,    // 10 27 xx
    CommonInstruction::BCC::create,    // 10 30 xx - BHIS is the same instruction as BCC
    CommonInstruction::BCC::create,    // 10 31 xx
    CommonInstruction::BCC::create,    // 10 32 xx
    CommonInstruction::BCC::create,    // 10 33 xx
    CommonInstruction::BCS::create,    // 10 34 xx - BLO is the same instruction as BCS
    CommonInstruction::BCS::create,    // 10 35 xx
    CommonInstruction::BCS::create,    // 10 36 xx
    CommonInstruction::BCS::create,    // 10 37 xx
    CommonInstruction::EMT::create,    // 10 40 xx
    CommonInstruction::EMT::create,    // 10 41 xx
    CommonInstruction::EMT::create,    // 10 42 xx
    CommonInstruction::EMT::create,    // 10 43 xx
    CommonInstruction::TRAP::create,   // 10 44 xx
    CommonInstruction::TRAP::create,   // 10 45 xx
    CommonInstruction::TRAP::create,   // 10 46 xx
    CommonInstruction::TRAP::create,   // 10 47 xx
    KDF11_AInstruction::CLRB::create,  // 10 50 xx
    KDF11_AInstruction::COMB::create,  // 10 51 xx
    KDF11_AInstruction::INCB::create,  // 10 52 xx
    KDF11_AInstruction::DECB::create,  // 10 53 xx
    KDF11_AInstruction::NEGB::create,  // 10 54 xx
    KDF11_AInstruction::ADCB::create,  // 10 55 xx
    KDF11_AInstruction::SBCB::create,  // 10 56 xx
    CommonInstruction::TSTB::create,   // 10 57 xx
    KDF11_AInstruction::RORB::create,  // 10 60 xx
    KDF11_AInstruction::ROLB::create,  // 10 61 xx
    KDF11_AInstruction::ASRB::create,  // 10 62 xx
    KDF11_AInstruction::ASLB::create,  // 10 63 xx
    CommonInstruction::MTPS::create,   // 10 64 xx
    KDF11_AInstruction::MFPD::create,  // 10 65 xx MFPD and MFPI are identical instructions
    KDF11_AInstruction::MTPD::create,  // 10 66 xx MTPD and MTPI are identical instructions
    CommonInstruction::MFPS::create,   // 10 67 xx
    CommonInstruction::Unused::create, // 10 70 xx The range 10 70 xx till 10 77 xx is undefined
    CommonInstruction::Unused::create, // 10 71 xx in the LSI-11 Processor Handbook
    CommonInstruction::Unused::create, // 10 72 xx
    CommonInstruction::Unused::create, // 10 73 xx
    CommonInstruction::Unused::create, // 10 74 xx
    CommonInstruction::Unused::create, // 10 75 xx
    CommonInstruction::Unused::create, // 10 76 xx
    CommonInstruction::Unused::create, // 10 77 xx
};

// Instruction format for the Program Control and Single Operand Group
// instructions. 
KDF11_AInstruction::opCodeTable const KDF11_AInstruction::group_00_nn_xx
{ 
    decodeGroup_00_00_nn,                              // 00 00 NN
    CommonInstruction::JMP::create,                    // 00 01 xx
    decodeGroup_00_02_nn,                              // 00 02 NN
    KDF11_AInstruction::SWAB::create,                  // 00 03 xx
    CommonInstruction::BR::create,                     // 00 04 xx
    CommonInstruction::BR::create,                     // 00 05 xx
    CommonInstruction::BR::create,                     // 00 06 xx
    CommonInstruction::BR::create,                     // 00 07 xx
    CommonInstruction::BNE::create,                    // 00 10 xx
    CommonInstruction::BNE::create,                    // 00 11 xx
    CommonInstruction::BNE::create,                    // 00 12 xx
    CommonInstruction::BNE::create,                    // 00 13 xx
    CommonInstruction::BEQ::create,                    // 00 14 xx
    CommonInstruction::BEQ::create,                    // 00 15 xx
    CommonInstruction::BEQ::create,                    // 00 16 xx
    CommonInstruction::BEQ::create,                    // 00 17 xx
    CommonInstruction::BGE::create,                    // 00 20 xx
    CommonInstruction::BGE::create,                    // 00 21 xx
    CommonInstruction::BGE::create,                    // 00 22 xx
    CommonInstruction::BGE::create,                    // 00 23 xx
    CommonInstruction::BLT::create,                    // 00 24 xx
    CommonInstruction::BLT::create,                    // 00 25 xx
    CommonInstruction::BLT::create,                    // 00 26 xx
    CommonInstruction::BLT::create,                    // 00 27 xx
    CommonInstruction::BGT::create,                    // 00 30 xx
    CommonInstruction::BGT::create,                    // 00 31 xx
    CommonInstruction::BGT::create,                    // 00 32 xx
    CommonInstruction::BGT::create,                    // 00 33 xx
    CommonInstruction::BLE::create,                    // 00 34 xx
    CommonInstruction::BLE::create,                    // 00 35 xx
    CommonInstruction::BLE::create,                    // 00 36 xx
    CommonInstruction::BLE::create,                    // 00 37 xx
    CommonInstruction::JSR::create,                    // 00 40 xx
    CommonInstruction::JSR::create,                    // 00 41 xx
    CommonInstruction::JSR::create,                    // 00 42 xx
    CommonInstruction::JSR::create,                    // 00 43 xx
    CommonInstruction::JSR::create,                    // 00 44 xx
    CommonInstruction::JSR::create,                    // 00 45 xx
    CommonInstruction::JSR::create,                    // 00 46 xx
    CommonInstruction::JSR::create,                    // 00 47 xx
    KDF11_AInstruction::CLR::create,                   // 00 50 xx
    KDF11_AInstruction::COM::create,                   // 00 51 xx
    KDF11_AInstruction::INC::create,                   // 00 52 xx
    KDF11_AInstruction::DEC::create,                   // 00 53 xx
    KDF11_AInstruction::NEG::create,                   // 00 54 xx
    KDF11_AInstruction::ADC::create,                   // 00 55 xx
    KDF11_AInstruction::SBC::create,                   // 00 56 xx
    CommonInstruction::TST::create,                    // 00 57 xx
    KDF11_AInstruction::ROR::create,                   // 00 60 xx
    KDF11_AInstruction::ROL::create,                   // 00 61 xx
    KDF11_AInstruction::ASR::create,                   // 00 62 xx
    KDF11_AInstruction::ASL::create,                   // 00 63 xx
    CommonInstruction::MARK::create,                   // 00 64 xx
    KDF11_AInstruction::MFPD::create,                  // 00 65 xx KDF11-A specific instruction
    KDF11_AInstruction::MTPD::create,                  // 00 66 xx idem
    KDF11_AInstruction::SXT::create,                   // 00 67 xx
    CommonInstruction::Unused::create,                 // 00 70 xx
    CommonInstruction::Unused::create,                 // 00 71 xx
    CommonInstruction::Unused::create,                 // 00 72 xx
    CommonInstruction::Unused::create,                 // 00 73 xx
    CommonInstruction::Unused::create,                 // 00 74 xx
    CommonInstruction::Unused::create,                 // 00 75 xx
    CommonInstruction::Unused::create,                 // 00 76 xx
    CommonInstruction::Unused::create,                 // 00 77 xx
};

// This is the main operation code control table.
KDF11_AInstruction::opCodeTable const KDF11_AInstruction::group_nn_xx_xx 
{
    decodeGroup_00_nn_xx,                              // 00 xx xx
    KDF11_AInstruction::MOV::create,                   // 01 xx xx
    CommonInstruction::CMP::create,                    // 02 xx xx
    CommonInstruction::BIT::create,                    // 03 xx xx
    KDF11_AInstruction::BIC::create,                   // 04 xx xx
    KDF11_AInstruction::BIS::create,                   // 05 xx xx
    KDF11_AInstruction::ADD::create,                   // 06 xx xx
    decodeGroup_07_nx_xx,                              // 07 nx xx
    decodeGroup_10_xx_xx,                              // 10 xx xx
    KDF11_AInstruction::MOVB::create,                  // 11 xx xx
    CommonInstruction::CMPB::create,                   // 12 xx xx
    CommonInstruction::BITB::create,                   // 13 xx xx
    KDF11_AInstruction::BICB::create,                  // 14 xx xx
    KDF11_AInstruction::BISB::create,                   // 15 xx xx
    KDF11_AInstruction::SUB::create,                   // 16 xx xx
    CommonInstruction::Unused::create                  // 17 xx xx Undefined in the LSI-11 Processor Handbook
};