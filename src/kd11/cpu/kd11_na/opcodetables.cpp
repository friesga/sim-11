#include "kd11_na.h"
#include "adc.h"
#include "adcb.h"
#include "add.h"
#include "ash.h"
#include "ashc.h"
#include "asl.h"
#include "aslb.h"
#include "asr.h"
#include "asrb.h"
#include "bcc.h"
#include "bcs.h"
#include "beq.h"
#include "bge.h"
#include "bgt.h"
#include "bhi.h"
#include "bic.h"
#include "bicb.h"
#include "bis.h"
#include "bisb.h"
#include "bit.h"
#include "bitb.h"
#include "ble.h"
#include "blos.h"
#include "blt.h"
#include "bmi.h"
#include "bne.h"
#include "bpl.h"
#include "bpt.h"
#include "br.h"
#include "bvc.h"
#include "bvs.h"
#include "ccc.h"
#include "clr.h"
#include "clrb.h"
#include "cmp.h"
#include "cmpb.h"
#include "com.h"
#include "comb.h"
#include "dec.h"
#include "decb.h"
#include "div.h"
#include "emt.h"
#include "fadd.h"
#include "fdiv.h"
#include "fmul.h"
#include "fsub.h"
#include "halt.h"
#include "inc.h"
#include "incb.h"
#include "iot.h"
#include "jmp.h"
#include "jsr.h"
#include "mark.h"
#include "mfps.h"
#include "mov.h"
#include "movb.h"
#include "mtps.h"
#include "mul.h"
#include "neg.h"
#include "negb.h"
#include "reset.h"
#include "rol.h"
#include "rolb.h"
#include "ror.h"
#include "rorb.h"
#include "rti.h"
#include "rts.h"
#include "rtt.h"
#include "sbc.h"
#include "sbcb.h"
#include "scc.h"
#include "sob.h"
#include "sub.h"
#include "swab.h"
#include "sxt.h"
#include "trap.h"
#include "tst.h"
#include "tstb.h"
#include "unused.h"
#include "wait.h"
#include "xor.h"

// Instruction format for Operate Group plus BPT and IOT,
//
// and minus NOP instructions.
KD11_NAInstruction::opCodeTable const KD11_NAInstruction::group_00_00_nn
{ 
    HALT::create,       // 00 00 00 
    WAIT::create,       // 00 00 01
    RTI::create,        // 00 00 02
    BPT::create,        // 00 00 03
    IOT::create,        // 00 00 04
    RESET::create,      // 00 00 05
    RTT::create         // 00 00 06
};

// Instruction format for RTS and Condition Code Operators instructions
KD11_NAInstruction::opCodeTable const KD11_NAInstruction::group_00_02_nn
{
    RTS::create,        // 00 02 00
    RTS::create,        // 00 02 01
    RTS::create,        // 00 02 02
    RTS::create,        // 00 02 03
    RTS::create,        // 00 02 04
    RTS::create,        // 00 02 05
    RTS::create,        // 00 02 06
    RTS::create,        // 00 02 07
    Unused::create,     // 00 02 10
    Unused::create,     // 00 02 11
    Unused::create,     // 00 02 12
    Unused::create,     // 00 02 13
    Unused::create,     // 00 02 14
    Unused::create,     // 00 02 15
    Unused::create,     // 00 02 16
    Unused::create,     // 00 02 17
    Unused::create,     // 00 02 20
    Unused::create,     // 00 02 21
    Unused::create,     // 00 02 22
    Unused::create,     // 00 02 23
    Unused::create,     // 00 02 24
    Unused::create,     // 00 02 25
    Unused::create,     // 00 02 26
    Unused::create,     // 00 02 27
    Unused::create,     // 00 02 30
    Unused::create,     // 00 02 31
    Unused::create,     // 00 02 32
    Unused::create,     // 00 02 33
    Unused::create,     // 00 02 34
    Unused::create,     // 00 02 35
    Unused::create,     // 00 02 36
    Unused::create,     // 00 02 37
    CCC::create,        // 00 02 40
    CCC::create,        // 00 02 41
    CCC::create,        // 00 02 42
    CCC::create,        // 00 02 43
    CCC::create,        // 00 02 44
    CCC::create,        // 00 02 45
    CCC::create,        // 00 02 46
    CCC::create,        // 00 02 47
    CCC::create,        // 00 02 50
    CCC::create,        // 00 02 51
    CCC::create,        // 00 02 52
    CCC::create,        // 00 02 53
    CCC::create,        // 00 02 54
    CCC::create,        // 00 02 55
    CCC::create,        // 00 02 56
    CCC::create,        // 00 02 57
    SCC::create,        // 00 02 60
    SCC::create,        // 00 02 61
    SCC::create,        // 00 02 62
    SCC::create,        // 00 02 63
    SCC::create,        // 00 02 64
    SCC::create,        // 00 02 65
    SCC::create,        // 00 02 66
    SCC::create,        // 00 02 67
    SCC::create,        // 00 02 70
    SCC::create,        // 00 02 71
    SCC::create,        // 00 02 72
    SCC::create,        // 00 02 73
    SCC::create,        // 00 02 74
    SCC::create,        // 00 02 75
    SCC::create,        // 00 02 76
    SCC::create         // 00 02 77
};

// Instruction format for Fixed and Floating Point Arithmetic instructions
// (just the FIS part of the instruction format).
// 
// Just the operation codes in the range 07 50 0x till 07 50 3x are used.
// The range 07 51 xx till 07 57 xx is handled in the corresponding function
// to minimize the size of this opcode table.
//
KD11_NAInstruction::opCodeTable const KD11_NAInstruction::group_07_5n_nx
{
    FADD::create,       // 07 50 0x
    FSUB::create,       // 07 50 1x
    FMUL::create,       // 07 50 2x
    FDIV::create,       // 07 50 3x
    Unused::create,     // 07 50 4x
    Unused::create,     // 07 50 5x
    Unused::create,     // 07 50 6x
    Unused::create      // 07 50 7x
};

// Instruction format for Fixed and Floating Point Arithmetic instructions,
// plus the SOB and XOR instructions.
// 
// The FIS instructions are defined in a separate group.
//
KD11_NAInstruction::opCodeTable const KD11_NAInstruction::group_07_nx_xx
{
    MUL::create,                    // 07 0x xx
    DIV::create,                    // 07 1x xx
    ASH::create,                    // 07 2x xx
    ASHC::create,                   // 07 3x xx
    XOR::create,                    // 07 4x xx
    decodeGroup_07_5n_nx,           // 07 5x xx
    Unused::create,                 // 07 6x xx
    SOB::create                     // 07 7x xx
};

// Instruction format for the Program Control and Single Operand Group
// instructions.
KD11_NAInstruction::opCodeTable const KD11_NAInstruction::group_10_xx_xx
{
    BPL::create,    // 10 00 xx
    BPL::create,    // 10 01 xx
    BPL::create,    // 10 02 xx
    BPL::create,    // 10 03 xx
    BMI::create,    // 10 04 xx
    BMI::create,    // 10 05 xx
    BMI::create,    // 10 06 xx
    BMI::create,    // 10 07 xx
    BHI::create,    // 10 10 xx
    BHI::create,    // 10 11 xx
    BHI::create,    // 10 12 xx
    BHI::create,    // 10 13 xx
    BLOS::create,   // 10 14 xx
    BLOS::create,   // 10 15 xx
    BLOS::create,   // 10 16 xx
    BLOS::create,   // 10 17 xx
    BVC::create,    // 10 20 xx
    BVC::create,    // 10 21 xx
    BVC::create,    // 10 22 xx
    BVC::create,    // 10 23 xx
    BVS::create,    // 10 24 xx
    BVS::create,    // 10 25 xx
    BVS::create,    // 10 26 xx
    BVS::create,    // 10 27 xx
    BCC::create,    // 10 30 xx - BHIS is the same instruction as BCC
    BCC::create,    // 10 31 xx
    BCC::create,    // 10 32 xx
    BCC::create,    // 10 33 xx
    BCS::create,    // 10 34 xx - BLO is the same instruction as BCS
    BCS::create,    // 10 35 xx
    BCS::create,    // 10 36 xx
    BCS::create,    // 10 37 xx
    EMT::create,    // 10 40 xx
    EMT::create,    // 10 41 xx
    EMT::create,    // 10 42 xx
    EMT::create,    // 10 43 xx
    TRAP::create,   // 10 44 xx
    TRAP::create,   // 10 45 xx
    TRAP::create,   // 10 46 xx
    TRAP::create,   // 10 47 xx
    CLRB::create,   // 10 50 xx
    COMB::create,   // 10 51 xx
    INCB::create,   // 10 52 xx
    DECB::create,   // 10 53 xx
    NEGB::create,   // 10 54 xx
    ADCB::create,   // 10 55 xx
    SBCB::create,   // 10 56 xx
    TSTB::create,   // 10 57 xx
    RORB::create,   // 10 60 xx
    ROLB::create,   // 10 61 xx
    ASRB::create,   // 10 62 xx
    ASLB::create,   // 10 63 xx
    MTPS::create,   // 10 64 xx
    Unused::create, // 10 65 xx Undefined in the LSI-11 Processor Handbook
    Unused::create, // 10 66 xx Undefined in the LSI-11 Processor Handbook
    MFPS::create,   // 10 67 xx
    Unused::create, // 10 70 xx The range 10 70 xx till 10 77 xx is undefined
    Unused::create, // 10 71 xx in the LSI-11 Processor Handbook
    Unused::create, // 10 72 xx
    Unused::create, // 10 73 xx
    Unused::create, // 10 74 xx
    Unused::create, // 10 75 xx
    Unused::create, // 10 76 xx
    Unused::create, // 10 77 xx
};

// Instruction format for the Program Control and Single Operand Group
// instructions. 
KD11_NAInstruction::opCodeTable const KD11_NAInstruction::group_00_nn_xx
{ 
    decodeGroup_00_00_nn,           // 00 00 NN
    JMP::create,                    // 00 01 xx
    decodeGroup_00_02_nn,           // 00 02 NN
    SWAB::create,                   // 00 03 xx
    BR::create,                     // 00 04 xx
    BR::create,                     // 00 05 xx
    BR::create,                     // 00 06 xx
    BR::create,                     // 00 07 xx
    BNE::create,                    // 00 10 xx
    BNE::create,                    // 00 11 xx
    BNE::create,                    // 00 12 xx
    BNE::create,                    // 00 13 xx
    BEQ::create,                    // 00 14 xx
    BEQ::create,                    // 00 15 xx
    BEQ::create,                    // 00 16 xx
    BEQ::create,                    // 00 17 xx
    BGE::create,                    // 00 20 xx
    BGE::create,                    // 00 21 xx
    BGE::create,                    // 00 22 xx
    BGE::create,                    // 00 23 xx
    BLT::create,                    // 00 24 xx
    BLT::create,                    // 00 25 xx
    BLT::create,                    // 00 26 xx
    BLT::create,                    // 00 27 xx
    BGT::create,                    // 00 30 xx
    BGT::create,                    // 00 31 xx
    BGT::create,                    // 00 32 xx
    BGT::create,                    // 00 33 xx
    BLE::create,                    // 00 34 xx
    BLE::create,                    // 00 35 xx
    BLE::create,                    // 00 36 xx
    BLE::create,                    // 00 37 xx
    JSR::create,                    // 00 40 xx
    JSR::create,                    // 00 41 xx
    JSR::create,                    // 00 42 xx
    JSR::create,                    // 00 43 xx
    JSR::create,                    // 00 44 xx
    JSR::create,                    // 00 45 xx
    JSR::create,                    // 00 46 xx
    JSR::create,                    // 00 47 xx
    CLR::create,                    // 00 50 xx
    COM::create,                    // 00 51 xx
    INC::create,                    // 00 52 xx
    DEC::create,                    // 00 53 xx
    NEG::create,                    // 00 54 xx
    ADC::create,                    // 00 55 xx
    SBC::create,                    // 00 56 xx
    TST::create,                    // 00 57 xx
    ROR::create,                    // 00 60 xx
    ROL::create,                    // 00 61 xx
    ASR::create,                    // 00 62 xx
    ASL::create,                    // 00 63 xx
    MARK::create,                   // 00 64 xx
    Unused::create,                 // 00 65 xx Undefined in the LSI-11 Processor Handbook
    Unused::create,                 // 00 66 xx idem
    SXT::create,                    // 00 67 xx
    Unused::create,                 // 00 70 xx
    Unused::create,                 // 00 71 xx
    Unused::create,                 // 00 72 xx
    Unused::create,                 // 00 73 xx
    Unused::create,                 // 00 74 xx
    Unused::create,                 // 00 75 xx
    Unused::create,                 // 00 76 xx
    Unused::create,                 // 00 77 xx
};

// This is the main operation code control table.
KD11_NAInstruction::opCodeTable const KD11_NAInstruction::group_nn_xx_xx 
{
    decodeGroup_00_nn_xx,           // 00 xx xx
    MOV::create,                    // 01 xx xx
    CMP::create,                    // 02 xx xx
    BIT::create,                    // 03 xx xx
    BIC::create,                    // 04 xx xx
    BIS::create,                    // 05 xx xx
    ADD::create,                    // 06 xx xx
    decodeGroup_07_nx_xx,           // 07 nx xx
    decodeGroup_10_xx_xx,           // 10 xx xx
    MOVB::create,                   // 11 xx xx
    CMPB::create,                   // 12 xx xx
    BITB::create,                   // 13 xx xx
    BICB::create,                   // 14 xx xx
    BISB::create,                   // 15 xx xx
    SUB::create,                    // 16 xx xx
    Unused::create                  // 17 xx xx Undefined in the LSI-11 Processor Handbook
};