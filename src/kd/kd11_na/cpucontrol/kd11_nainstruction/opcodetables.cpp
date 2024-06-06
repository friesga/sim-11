#include "kd11_nainstruction.h"
#include "kd/common/instructions/commoninstruction.h"
#include "instructions.h"

// Instruction format for Operate Group plus BPT and IOT,
//
// and minus NOP instructions.
KD11_NAInstruction::opCodeTable const KD11_NAInstruction::group_00_00_nn
{ 
    create<CommonInstruction::HALT>,       // 00 00 00 
    create<CommonInstruction::WAIT>,       // 00 00 01
    create<CommonInstruction::RTI>,        // 00 00 02
    create<CommonInstruction::BPT>,        // 00 00 03
    create<CommonInstruction::IOT>,        // 00 00 04
    create<CommonInstruction::RESET>,      // 00 00 05
    create<CommonInstruction::RTT>         // 00 00 06
};

// Instruction format for RTS and Condition Code Operators instructions
KD11_NAInstruction::opCodeTable const KD11_NAInstruction::group_00_02_nn
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

// Instruction format for Fixed and Floating Point Arithmetic instructions
// (just the FIS part of the instruction format).
// 
// Just the operation codes in the range 07 50 0x till 07 50 3x are used.
// The range 07 51 xx till 07 57 xx is handled in the corresponding function
// to minimize the size of this opcode table.
//
KD11_NAInstruction::opCodeTable const KD11_NAInstruction::group_07_5n_nx
{
    create<CommonInstruction::FADD>,       // 07 50 0x
    create<CommonInstruction::FSUB>,       // 07 50 1x
    create<CommonInstruction::FMUL>,       // 07 50 2x
    create<CommonInstruction::FDIV>,       // 07 50 3x
    create<CommonInstruction::Unused>,     // 07 50 4x
    create<CommonInstruction::Unused>,     // 07 50 5x
    create<CommonInstruction::Unused>,     // 07 50 6x
    create<CommonInstruction::Unused>      // 07 50 7x
};

// Instruction format for Fixed and Floating Point Arithmetic instructions,
// plus the SOB and XOR instructions.
// 
// The FIS instructions are defined in a separate group.
//
KD11_NAInstruction::opCodeTable const KD11_NAInstruction::group_07_nx_xx
{
    create<CommonInstruction::MUL>,                    // 07 0x xx
    create<CommonInstruction::DIV>,                    // 07 1x xx
    create<CommonInstruction::ASH>,                    // 07 2x xx
    create<CommonInstruction::ASHC>,                   // 07 3x xx
    create<CommonInstruction::XOR>,                    // 07 4x xx
    decodeGroup_07_5n_nx,                              // 07 5x xx
    create<CommonInstruction::Unused>,                 // 07 6x xx
    create<CommonInstruction::SOB>                     // 07 7x xx
};

// Instruction format for the Program Control and Single Operand Group
// instructions.
KD11_NAInstruction::opCodeTable const KD11_NAInstruction::group_10_xx_xx
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
    create<CommonInstruction::CLRB>,   // 10 50 xx
    create<CommonInstruction::COMB>,   // 10 51 xx
    create<CommonInstruction::INCB>,   // 10 52 xx
    create<CommonInstruction::DECB>,   // 10 53 xx
    create<CommonInstruction::NEGB>,   // 10 54 xx
    create<CommonInstruction::ADCB>,   // 10 55 xx
    create<CommonInstruction::SBCB>,   // 10 56 xx
    create<CommonInstruction::TSTB>,   // 10 57 xx
    create<CommonInstruction::RORB>,   // 10 60 xx
    create<CommonInstruction::ROLB>,   // 10 61 xx
    create<CommonInstruction::ASRB>,   // 10 62 xx
    create<CommonInstruction::ASLB>,   // 10 63 xx
    create<CommonInstruction::MTPS>,   // 10 64 xx
    create<CommonInstruction::Unused>, // 10 65 xx Undefined in the LSI-11 Processor Handbook
    create<CommonInstruction::Unused>, // 10 66 xx Undefined in the LSI-11 Processor Handbook
    create<CommonInstruction::MFPS>,   // 10 67 xx
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
KD11_NAInstruction::opCodeTable const KD11_NAInstruction::group_00_nn_xx
{ 
    decodeGroup_00_00_nn,                              // 00 00 NN
    create<CommonInstruction::JMP>,                    // 00 01 xx
    decodeGroup_00_02_nn,                              // 00 02 NN
    create<CommonInstruction::SWAB>,                   // 00 03 xx
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
    create<CommonInstruction::CLR>,                    // 00 50 xx
    create<CommonInstruction::COM>,                    // 00 51 xx
    create<CommonInstruction::INC>,                    // 00 52 xx
    create<CommonInstruction::DEC>,                    // 00 53 xx
    create<CommonInstruction::NEG>,                    // 00 54 xx
    create<CommonInstruction::ADC>,                    // 00 55 xx
    create<CommonInstruction::SBC>,                    // 00 56 xx
    create<CommonInstruction::TST>,                    // 00 57 xx
    create<CommonInstruction::ROR>,                    // 00 60 xx
    create<CommonInstruction::ROL>,                    // 00 61 xx
    create<CommonInstruction::ASR>,                    // 00 62 xx
    create<CommonInstruction::ASL>,                    // 00 63 xx
    create<CommonInstruction::MARK>,                   // 00 64 xx
    create<CommonInstruction::Unused>,                 // 00 65 xx Undefined in the LSI-11 Processor Handbook
    create<CommonInstruction::Unused>,                 // 00 66 xx idem
    create<CommonInstruction::SXT>,                    // 00 67 xx
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
KD11_NAInstruction::opCodeTable const KD11_NAInstruction::group_nn_xx_xx 
{
    decodeGroup_00_nn_xx,                              // 00 xx xx
    create<CommonInstruction::MOV>,                    // 01 xx xx
    create<CommonInstruction::CMP>,                    // 02 xx xx
    create<CommonInstruction::BIT>,                    // 03 xx xx
    create<CommonInstruction::BIC>,                    // 04 xx xx
    create<CommonInstruction::BIS>,                    // 05 xx xx
    create<CommonInstruction::ADD>,                    // 06 xx xx
    decodeGroup_07_nx_xx,                              // 07 nx xx
    decodeGroup_10_xx_xx,                              // 10 xx xx
    create<CommonInstruction::MOVB>,                   // 11 xx xx
    create<CommonInstruction::CMPB>,                   // 12 xx xx
    create<CommonInstruction::BITB>,                   // 13 xx xx
    create<CommonInstruction::BICB>,                   // 14 xx xx
    create<CommonInstruction::BISB>,                   // 15 xx xx
    create<CommonInstruction::SUB>,                    // 16 xx xx
    create<CommonInstruction::Unused>                  // 17 xx xx Undefined in the LSI-11 Processor Handbook
};