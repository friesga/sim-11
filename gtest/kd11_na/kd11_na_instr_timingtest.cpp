#include "kd/kd11_na/calculate/calculate.h"
#include "kd/common/instructions/instructions.h"

#include <gtest/gtest.h>

// Definition of the (dummy) test fixture
class KD11_NA_InstrTimingTest : public ::testing::Test
{};

// Single operand instructions
//
// This set of tests verifies the DestinationTimes table for word instructions
TEST_F (KD11_NA_InstrTimingTest, CLR)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CLR {005000}}), 3.85);  // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CLR {005010}}), 6.3);   // Mode 1
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CLR {005020}}), 6.3);   // Mode 2
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CLR {005030}}), 8.4);   // Mode 3
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CLR {005040}}), 7.0);   // Mode 4
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CLR {005050}}), 9.1);   // Mode 5
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CLR {005060}}), 9.1);   // Mode 6
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CLR {005070}}), 10.85); // Mode 7
}

// The timing for the CLRB instruction uses the DestinationTimes table for bytes instructions
TEST_F (KD11_NA_InstrTimingTest, CLRB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CLRB {0105000}}), 3.85);    // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CLRB {0105010}}), 5.95);    // Mode 1
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CLRB {0105020}}), 5.95);    // Mode 2
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CLRB {0105030}}), 8.4);     // Mode 3
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CLRB {0105040}}), 6.65);    // Mode 4
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CLRB {0105050}}), 9.1);     // Mode 5
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CLRB {0105060}}), 8.75);    // Mode 6
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CLRB {0105070}}), 11.2);    // Mode 7
}

TEST_F (KD11_NA_InstrTimingTest, COM)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {COM {005100}}), 4.2);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {COM {005110}}), 6.65);  // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, COMB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {COMB {0105100}}), 3.85);    // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {COMB {0105110}}), 5.95);    // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, INC)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {INC {005200}}), 4.2);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {INC {005210}}), 7.0);   // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, INCB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {INCB {0105200}}), 3.85);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {INCB {0105210}}), 6.3);    // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, DEC)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {DEC {005300}}), 4.2);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {DEC {005310}}), 7.0);   // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, DECB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {DECB {0105300}}), 3.85);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {DECB {0105310}}), 6.3);    // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, NEG)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {NEG {005400}}), 4.2);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {NEG {005410}}), 6.65);  // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, NEGB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {NEGB {0105400}}), 3.85);    // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {NEGB {0105410}}), 5.95);    // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, TST)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {TST {005700}}), 4.2);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {TST {005710}}), 5.95);  // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, TSTB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {TSTB {0105700}}), 3.85);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {TSTB {0105710}}), 5.25);   // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, ASR)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ASR {005700}}), 5.6);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ASR {005710}}), 8.4);   // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, ASRB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ASRB {0105700}}), 4.55);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ASRB {0105710}}), 7.7);   // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, ASL)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ASL {006300}}), 3.85);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ASL {006310}}), 6.65);   // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, ASLB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ASLB {0106300}}), 3.85);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ASLB {0106310}}), 5.95);   // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, ROR)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ROR {006000}}), 5.25);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ROR {006010}}), 8.05);   // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, RORB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {RORB {0106000}}), 4.20);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {RORB {0106010}}), 6.65);   // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, ROL)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ROL {006100}}), 3.85);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ROL {006110}}), 6.65);   // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, ROLB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ROLB {0106100}}), 3.85);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ROLB {0106110}}), 5.95);   // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, SWAB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {SWAB {000300}}), 4.2);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {SWAB {000310}}), 5.95);  // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, ADC)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ADC {005500}}), 4.2);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ADC {005510}}), 7.0);   // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, ADCB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ADCB {0105500}}), 3.85);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ADCB {0105510}}), 6.3);    // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, SBC)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {SBC {005600}}), 4.2);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {SBC {005610}}), 7.0);   // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, SBCB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {SBCB {0105600}}), 3.85);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {SBCB {0105610}}), 6.3);    // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, SXT)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {SXT {006700}}), 5.95);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {SXT {006710}}), 8.75);   // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, MFPS)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MFPS {0106700}}), 4.90);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MFPS {0106710}}), 8.40);   // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, MTPS)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MTPS {0106400}}), 7.00);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MTPS {0106410}}), 8.75);   // Mode 1
}

// For the JMP instruction addressing mode 0 is not used.
TEST_F (KD11_NA_InstrTimingTest, JMP)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {JMP {000110}}), 4.20);   // Mode 1
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {JMP {000120}}), 4.90);   // Mode 2
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {JMP {000130}}), 5.60);   // Mode 3
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {JMP {000140}}), 4.90);   // Mode 4
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {JMP {000150}}), 6.30);   // Mode 5
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {JMP {000160}}), 6.30);   // Mode 6
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {JMP {000170}}), 8.40);   // Mode 7
}

// The execution time of the MARK instruction is the same for all modes
TEST_F (KD11_NA_InstrTimingTest, MARK)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MARK {006400}}), 11.55);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MARK {006410}}), 11.55);   // Mode 1
}

// Double operand instructions
//
// This set of tests verifies the SourceTimes table for word instructions
TEST_F (KD11_NA_InstrTimingTest, MOV)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOV {010000}}), 3.5);   // Source mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOV {011000}}), 4.9);   // Source mode 1
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOV {012000}}), 4.9);   // Source mode 2
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOV {013000}}), 7.0);   // Source mode 3
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOV {014000}}), 5.6);   // Source mode 4
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOV {015000}}), 7.7);   // Source mode 5
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOV {016700}}), 7.7);   // Source mode 6
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOV {017000}}), 9.8);   // Source mode 7
}

TEST_F (KD11_NA_InstrTimingTest, MOVB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOVB {0110000}}), 3.85);   // Mode 0/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOVB {0111000}}), 4.9);    // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOVB {0112000}}), 4.9);    // Mode 2/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOVB {0113000}}), 7.0);    // Mode 3/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOVB {0114000}}), 5.6);    // Mode 4/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOVB {0115000}}), 7.7);    // Mode 5/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOVB {0116000}}), 7.7);    // Mode 6/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOVB {0117000}}), 9.8);    // Mode 7/0

    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOVB {0110010}}), 5.60);   // Mode 0/1
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOVB {0110020}}), 5.60);   // Mode 0/2
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOVB {0110030}}), 8.05);   // Mode 0/3
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOVB {0110040}}), 6.30);   // Mode 0/4
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOVB {0110050}}), 8.75);   // Mode 0/5
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOVB {0110060}}), 8.40);   // Mode 0/6
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MOVB {0110070}}), 10.85);  // Mode 0/7
}

TEST_F (KD11_NA_InstrTimingTest, CMP)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CMP {020000}}), 3.5);   // Mode 0/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CMP {021000}}), 4.9);   // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CMP {020010}}), 5.25);  // Mode 0/1
}

TEST_F (KD11_NA_InstrTimingTest, CMPB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CMPB {0120000}}), 3.15);   // Mode 0/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CMPB {0121000}}), 4.2);    // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CMPB {0120010}}), 4.55);   // Mode 0/1
}

TEST_F (KD11_NA_InstrTimingTest, ADD)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ADD {060000}}), 3.50);   // Mode 0/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ADD {061000}}), 4.90);   // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ADD {060010}}), 6.30);   // Mode 0/1
}

TEST_F (KD11_NA_InstrTimingTest, SUB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {SUB {0160000}}), 3.50);   // Mode 0/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {SUB {0161000}}), 4.90);   // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {SUB {0160010}}), 6.30);   // Mode 0/1
}

TEST_F (KD11_NA_InstrTimingTest, BIT)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BIT {030000}}), 3.5);   // Mode 0/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BIT {031000}}), 4.9);   // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BIT {030010}}), 5.25);  // Mode 0/1
}

TEST_F (KD11_NA_InstrTimingTest, BITB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BITB {0130000}}), 3.15);   // Mode 0/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BITB {0131000}}), 4.2);    // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BITB {0130010}}), 4.55);   // Mode 0/1
}

TEST_F (KD11_NA_InstrTimingTest, BIC)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BIC {040000}}), 3.50);   // Mode 0/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BIC {041000}}), 4.90);   // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BIC {040010}}), 6.30);   // Mode 0/1
}

TEST_F (KD11_NA_InstrTimingTest, BICB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BICB {0110000}}), 3.85);   // Mode 0/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BICB {0111000}}), 4.9);    // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BICB {0110010}}), 5.60);   // Mode 0/1
}

TEST_F (KD11_NA_InstrTimingTest, BIS)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BIS {050000}}), 3.50);   // Mode 0/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BIS {051000}}), 4.90);   // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BIS {050010}}), 6.30);   // Mode 0/1
}

TEST_F (KD11_NA_InstrTimingTest, BISB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BISB {0110000}}), 3.85);   // Mode 0/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BISB {0111000}}), 4.9);    // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BISB {0110010}}), 5.60);   // Mode 0/1
}

// Eis instructions, including JSR and XOR
// 
// For the JSR instruction addressing mode 0 is not used. The timing table is
// tested for the JMP instruction.
TEST_F (KD11_NA_InstrTimingTest, JSR)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {JSR {004010}}), 7.1);    // Mode 1, PC != LINK
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {JSR {004710}}), 5.95);   // Mode 1, PC == LINK
}

TEST_F (KD11_NA_InstrTimingTest, MUL)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {MUL {070000}}), 40.0);   // Assumption
}

TEST_F (KD11_NA_InstrTimingTest, DIV)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {DIV {071000}}), 42.0);   // Assumption
}

TEST_F (KD11_NA_InstrTimingTest, ASH)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ASH {072000}}), 10.1);   // No shift
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ASH {072001}}), 13.25);  // Shift 1 bit left
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ASH {072077}}), 11.85);  // Shift 1 bit right
}

TEST_F (KD11_NA_InstrTimingTest, ASHC)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ASHC {073000}}), 10.1);   // No shift
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ASHC {073001}}), 13.25);  // Shift 1 bit left
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {ASHC {073077}}), 12.90);  // Shift 1 bit right
}

TEST_F (KD11_NA_InstrTimingTest, XOR)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {XOR {074000}}), 3.50);   // Mode 0
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {XOR {074010}}), 6.30);   // Mode 1
}

TEST_F (KD11_NA_InstrTimingTest, SOB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {SOB {077000}}), 4.90);
}

// FIS format instructions
TEST_F (KD11_NA_InstrTimingTest, RTS)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {RTS {000200}}), 5.25);  
}

TEST_F (KD11_NA_InstrTimingTest, FADD)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {FADD {075000}}), 75.0);
}

TEST_F (KD11_NA_InstrTimingTest, FSUB)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {FSUB {075010}}), 75.0);
}

TEST_F (KD11_NA_InstrTimingTest, FMUL)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {FMUL {075020}}), 151.0);
}

TEST_F (KD11_NA_InstrTimingTest, FDIV)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {FDIV {075030}}), 151.0);
}

// Branch instructions
TEST_F (KD11_NA_InstrTimingTest, BranchInstructions)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BR {000400}}), 3.50);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BNE {001000}}), 3.50);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BEQ {001400}}), 3.50);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BPL {0100000}}), 3.50);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BMI {0100400}}), 3.50);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BVC {0102000}}), 3.50);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BVS {0102400}}), 3.50);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BCC {0103000}}), 3.50);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BCS {0103400}}), 3.50);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BGE {002000}}), 3.50);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BLT {002400}}), 3.50);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BGT {003000}}), 3.50);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BLE {003400}}), 3.50);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BHI {0101000}}), 3.50);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BLOS {0101400}}), 3.50);

    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {EMT {0104000}}), 16.80);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {TRAP {0104400}}), 16.80);
}

TEST_F (KD11_NA_InstrTimingTest, NoOperandInstructions)
{
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {HALT {000000}}), 5.60);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {WAIT {000001}}), 6.30);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {RTI {000002}}), 8.75);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {BPT {000003}}), 18.55);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {IOT {000004}}), 18.55);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {RESET {000005}}), 105.95);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {RTT {000006}}), 8.75);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {CCC {000240}}), 3.50);
    EXPECT_DOUBLE_EQ (visit (KD11_NA::Calculate {}, Instruction {SCC {000260}}), 3.50);
}