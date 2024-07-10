#include "decoder.h"

// Instruction format for Operate Group plus BPT and IOT,
//
// and minus NOP instructions.
Decoder::opCodeTable const Decoder::group_00_00_nn
{ 
    create<HALT>,       // 00 00 00 
    create<WAIT>,       // 00 00 01
    create<RTI>,        // 00 00 02
    create<BPT>,        // 00 00 03
    create<IOT>,        // 00 00 04
    create<RESET>,      // 00 00 05
    create<RTT>,        // 00 00 06
    create<MFPT>        // 00 00 07
};

// Instruction format for RTS and Condition Code Operators instructions
Decoder::opCodeTable const Decoder::group_00_02_nn
{
    create<RTS>,        // 00 02 00
    create<RTS>,        // 00 02 01
    create<RTS>,        // 00 02 02
    create<RTS>,        // 00 02 03
    create<RTS>,        // 00 02 04
    create<RTS>,        // 00 02 05
    create<RTS>,        // 00 02 06
    create<RTS>,        // 00 02 07
    create<Unused>,     // 00 02 10
    create<Unused>,     // 00 02 11
    create<Unused>,     // 00 02 12
    create<Unused>,     // 00 02 13
    create<Unused>,     // 00 02 14
    create<Unused>,     // 00 02 15
    create<Unused>,     // 00 02 16
    create<Unused>,     // 00 02 17
    create<Unused>,     // 00 02 20
    create<Unused>,     // 00 02 21
    create<Unused>,     // 00 02 22
    create<Unused>,     // 00 02 23
    create<Unused>,     // 00 02 24
    create<Unused>,     // 00 02 25
    create<Unused>,     // 00 02 26
    create<Unused>,     // 00 02 27
    create<Unused>,     // 00 02 30
    create<Unused>,     // 00 02 31
    create<Unused>,     // 00 02 32
    create<Unused>,     // 00 02 33
    create<Unused>,     // 00 02 34
    create<Unused>,     // 00 02 35
    create<Unused>,     // 00 02 36
    create<Unused>,     // 00 02 37
    create<CCC>,        // 00 02 40
    create<CCC>,        // 00 02 41
    create<CCC>,        // 00 02 42
    create<CCC>,        // 00 02 43
    create<CCC>,        // 00 02 44
    create<CCC>,        // 00 02 45
    create<CCC>,        // 00 02 46
    create<CCC>,        // 00 02 47
    create<CCC>,        // 00 02 50
    create<CCC>,        // 00 02 51
    create<CCC>,        // 00 02 52
    create<CCC>,        // 00 02 53
    create<CCC>,        // 00 02 54
    create<CCC>,        // 00 02 55
    create<CCC>,        // 00 02 56
    create<CCC>,        // 00 02 57
    create<SCC>,        // 00 02 60
    create<SCC>,        // 00 02 61
    create<SCC>,        // 00 02 62
    create<SCC>,        // 00 02 63
    create<SCC>,        // 00 02 64
    create<SCC>,        // 00 02 65
    create<SCC>,        // 00 02 66
    create<SCC>,        // 00 02 67
    create<SCC>,        // 00 02 70
    create<SCC>,        // 00 02 71
    create<SCC>,        // 00 02 72
    create<SCC>,        // 00 02 73
    create<SCC>,        // 00 02 74
    create<SCC>,        // 00 02 75
    create<SCC>,        // 00 02 76
    create<SCC>         // 00 02 77
};

// Instruction format for Fixed and Floating Point Arithmetic instructions
// (just the FIS part of the instruction format).
// 
// Just the operation codes in the range 07 50 0x till 07 50 3x are used.
// The range 07 51 xx till 07 57 xx is handled in the corresponding function
// to minimize the size of this opcode table.
//
Decoder::opCodeTable const Decoder::group_07_5n_nx
{
    create<FADD>,       // 07 50 0x
    create<FSUB>,       // 07 50 1x
    create<FMUL>,       // 07 50 2x
    create<FDIV>,       // 07 50 3x
    create<Unused>,     // 07 50 4x
    create<Unused>,     // 07 50 5x
    create<Unused>,     // 07 50 6x
    create<Unused>      // 07 50 7x
};

// Instruction format for Fixed and Floating Point Arithmetic instructions,
// plus the SOB and XOR instructions.
// 
// The FIS instructions are defined in a separate group.
//
Decoder::opCodeTable const Decoder::group_07_nx_xx
{
    create<MUL>,                    // 07 0x xx
    create<DIV>,                    // 07 1x xx
    create<ASH>,                    // 07 2x xx
    create<ASHC>,                   // 07 3x xx
    create<XOR>,                    // 07 4x xx
    decodeGroup_07_5n_nx,                              // 07 5x xx
    create<Unused>,                 // 07 6x xx
    create<SOB>                     // 07 7x xx
};

// Instruction format for the Program Control and Single Operand Group
// instructions.
Decoder::opCodeTable const Decoder::group_10_xx_xx
{
    create<BPL>,    // 10 00 xx
    create<BPL>,    // 10 01 xx
    create<BPL>,    // 10 02 xx
    create<BPL>,    // 10 03 xx
    create<BMI>,    // 10 04 xx
    create<BMI>,    // 10 05 xx
    create<BMI>,    // 10 06 xx
    create<BMI>,    // 10 07 xx
    create<BHI>,    // 10 10 xx
    create<BHI>,    // 10 11 xx
    create<BHI>,    // 10 12 xx
    create<BHI>,    // 10 13 xx
    create<BLOS>,   // 10 14 xx
    create<BLOS>,   // 10 15 xx
    create<BLOS>,   // 10 16 xx
    create<BLOS>,   // 10 17 xx
    create<BVC>,    // 10 20 xx
    create<BVC>,    // 10 21 xx
    create<BVC>,    // 10 22 xx
    create<BVC>,    // 10 23 xx
    create<BVS>,    // 10 24 xx
    create<BVS>,    // 10 25 xx
    create<BVS>,    // 10 26 xx
    create<BVS>,    // 10 27 xx
    create<BCC>,    // 10 30 xx - BHIS is the same instruction as BCC
    create<BCC>,    // 10 31 xx
    create<BCC>,    // 10 32 xx
    create<BCC>,    // 10 33 xx
    create<BCS>,    // 10 34 xx - BLO is the same instruction as BCS
    create<BCS>,    // 10 35 xx
    create<BCS>,    // 10 36 xx
    create<BCS>,    // 10 37 xx
    create<EMT>,    // 10 40 xx
    create<EMT>,    // 10 41 xx
    create<EMT>,    // 10 42 xx
    create<EMT>,    // 10 43 xx
    create<TRAP>,   // 10 44 xx
    create<TRAP>,   // 10 45 xx
    create<TRAP>,   // 10 46 xx
    create<TRAP>,   // 10 47 xx
    create<CLRB>,   // 10 50 xx
    create<COMB>,   // 10 51 xx
    create<INCB>,   // 10 52 xx
    create<DECB>,   // 10 53 xx
    create<NEGB>,   // 10 54 xx
    create<ADCB>,   // 10 55 xx
    create<SBCB>,   // 10 56 xx
    create<TSTB>,   // 10 57 xx
    create<RORB>,   // 10 60 xx
    create<ROLB>,   // 10 61 xx
    create<ASRB>,   // 10 62 xx
    create<ASLB>,   // 10 63 xx
    create<MTPS>,   // 10 64 xx
    create<MFPD>,   // 10 65 xx 
    create<MTPD>,   // 10 66 xx
    create<MFPS>,   // 10 67 xx
    create<Unused>, // 10 70 xx The range 10 70 xx till 10 77 xx is undefined
    create<Unused>, // 10 71 xx in the LSI-11 Processor Handbook
    create<Unused>, // 10 72 xx
    create<Unused>, // 10 73 xx
    create<Unused>, // 10 74 xx
    create<Unused>, // 10 75 xx
    create<Unused>, // 10 76 xx
    create<Unused>, // 10 77 xx
};

// Instruction format for the Program Control and Single Operand Group
// instructions. 
Decoder::opCodeTable const Decoder::group_00_nn_xx
{ 
    decodeGroup_00_00_nn,                              // 00 00 NN
    create<JMP>,                    // 00 01 xx
    decodeGroup_00_02_nn,                              // 00 02 NN
    create<SWAB>,                   // 00 03 xx
    create<BR>,                     // 00 04 xx
    create<BR>,                     // 00 05 xx
    create<BR>,                     // 00 06 xx
    create<BR>,                     // 00 07 xx
    create<BNE>,                    // 00 10 xx
    create<BNE>,                    // 00 11 xx
    create<BNE>,                    // 00 12 xx
    create<BNE>,                    // 00 13 xx
    create<BEQ>,                    // 00 14 xx
    create<BEQ>,                    // 00 15 xx
    create<BEQ>,                    // 00 16 xx
    create<BEQ>,                    // 00 17 xx
    create<BGE>,                    // 00 20 xx
    create<BGE>,                    // 00 21 xx
    create<BGE>,                    // 00 22 xx
    create<BGE>,                    // 00 23 xx
    create<BLT>,                    // 00 24 xx
    create<BLT>,                    // 00 25 xx
    create<BLT>,                    // 00 26 xx
    create<BLT>,                    // 00 27 xx
    create<BGT>,                    // 00 30 xx
    create<BGT>,                    // 00 31 xx
    create<BGT>,                    // 00 32 xx
    create<BGT>,                    // 00 33 xx
    create<BLE>,                    // 00 34 xx
    create<BLE>,                    // 00 35 xx
    create<BLE>,                    // 00 36 xx
    create<BLE>,                    // 00 37 xx
    create<JSR>,                    // 00 40 xx
    create<JSR>,                    // 00 41 xx
    create<JSR>,                    // 00 42 xx
    create<JSR>,                    // 00 43 xx
    create<JSR>,                    // 00 44 xx
    create<JSR>,                    // 00 45 xx
    create<JSR>,                    // 00 46 xx
    create<JSR>,                    // 00 47 xx
    create<CLR>,                    // 00 50 xx
    create<COM>,                    // 00 51 xx
    create<INC>,                    // 00 52 xx
    create<DEC>,                    // 00 53 xx
    create<NEG>,                    // 00 54 xx
    create<ADC>,                    // 00 55 xx
    create<SBC>,                    // 00 56 xx
    create<TST>,                    // 00 57 xx
    create<ROR>,                    // 00 60 xx
    create<ROL>,                    // 00 61 xx
    create<ASR>,                    // 00 62 xx
    create<ASL>,                    // 00 63 xx
    create<MARK>,                   // 00 64 xx
    create<MFPD>,                   // 00 65 xx 
    create<MTPD>,                   // 00 66 xx
    create<SXT>,                    // 00 67 xx
    create<Unused>,                 // 00 70 xx
    create<Unused>,                 // 00 71 xx
    create<Unused>,                 // 00 72 xx
    create<Unused>,                 // 00 73 xx
    create<Unused>,                 // 00 74 xx
    create<Unused>,                 // 00 75 xx
    create<Unused>,                 // 00 76 xx
    create<Unused>,                 // 00 77 xx
};

// This is the main operation code control table.
Decoder::opCodeTable const Decoder::group_nn_xx_xx 
{
    decodeGroup_00_nn_xx,                              // 00 xx xx
    create<MOV>,                    // 01 xx xx
    create<CMP>,                    // 02 xx xx
    create<BIT>,                    // 03 xx xx
    create<BIC>,                    // 04 xx xx
    create<BIS>,                    // 05 xx xx
    create<ADD>,                    // 06 xx xx
    decodeGroup_07_nx_xx,                              // 07 nx xx
    decodeGroup_10_xx_xx,                              // 10 xx xx
    create<MOVB>,                   // 11 xx xx
    create<CMPB>,                   // 12 xx xx
    create<BITB>,                   // 13 xx xx
    create<BICB>,                   // 14 xx xx
    create<BISB>,                   // 15 xx xx
    create<SUB>,                    // 16 xx xx
    create<Unused>                  // 17 xx xx Undefined in the LSI-11 Processor Handbook
};