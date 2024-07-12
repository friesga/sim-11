#include "kd/common/instructiondecoder/instructiondecoder.h"
#include "kd/kdf11/calculate/calculate.h"
#include "kd/common/instructions/instructions.h"

#include <gtest/gtest.h>
#include <variant>

using std::visit;

// Definition of the test fixture
class KDF11_InstrTimingTest : public ::testing::Test
{
public:
    InstructionDecoder decoder;
};

// Single operand instructions
//
TEST_F (KDF11_InstrTimingTest, SingleOperandInstructions)
{
    // Instructions using the MOV basic timing plus the MOV destination timing.
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {CLR {005000}}), 2.025);  // Mode 0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {CLR {005010}}), 4.05);   // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {CLR {005020}}), 4.05);   // Mode 2
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {CLR {005030}}), 5.175);  // Mode 3
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {CLR {005040}}), 4.05);   // Mode 4
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {CLR {005050}}), 5.475);  // Mode 5
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {CLR {005060}}), 5.475);  // Mode 6
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {CLR {005070}}), 6.9);    // Mode 7
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {CLRB {0105010}}), 4.05); // Mode 1   
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {COM {005110}}), 4.05);   // Mode 1   
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {COMB {0105110}}), 4.05); // Mode 1

    // Instructions using the MOV basic timing plus the ADD destination timing.
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {INC {005200}}), 2.025);  // Mode 0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {INC {005210}}), 4.875);  // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {INC {005220}}), 4.875);  // Mode 2
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {INC {005230}}), 6.3);    // Mode 3
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {INC {005240}}), 4.875);  // Mode 4
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {INC {005250}}), 6.6);    // Mode 5
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {INC {005260}}), 6.6);    // Mode 6
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {INC {005270}}), 8.025);  // Mode 7
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {INCB {0105210}}), 4.875);    // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {DEC {005310}}), 4.875);      // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {DECB {0105310}}), 4.875);    // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {NEG {005410}}), 4.875);      // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {NEGB {0105410}}), 4.875);    // Mode 1

    // Instructions using the MOV basic timing plus the CMP(B) destination timing.
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {TST {005710}}), 3.75);      // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {TSTB {0105710}}), 3.75);    // Mode 1

    // IUnstructions using the MOV basic timing plus the ADD destination timing.
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {ASR {006210}}), 4.875);     // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {ASRB {0106210}}), 4.875);   // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {ASL {006310}}), 4.875);     // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {ASLB {0106310}}), 4.875);   // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {ROR {006010}}), 4.875);     // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {RORB {0106010}}), 4.875);   // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {ROL {006110}}), 4.875);     // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {ROLB {0106110}}), 4.875);   // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {SWAB {00310}}), 4.875);     // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {ADC {0005510}}), 4.875);    // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {ADCB {0105510}}), 4.875);   // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {SBC {0005610}}), 4.875);    // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {SBCB {0105610}}), 4.875);   // Mode 1

    // Instructions using the MOV basic timing plus the MOV(B) destination timing.
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {SXT {006710}}), 4.05);       // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MFPS {0106710}}), 4.05);     // Mode 1

    // MTPS uses the MTPS basic timing plus MUL source address timing
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MTPS {0106410}}), 5.325);    // Mode 1

    // MFPD and MFPI use the MFPI basing timing plus the MUL source address timing.
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MFPD {006510}}), 5.775);     // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MFPI {0106510}}), 5.775);    // Mode 1

    // MTPD and MTPI use the MTPI basic timing plus the MOV(B) source address timing
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MTPD {006610}}), 6.75);     // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MTPI {0106610}}), 6.75);    // Mode 1

    // The JMP instruction has an addressing mode specific basic timing
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {JMP {000110}}), 2.325);     // Mode 1

    // The MARK instruction has an non addressing mode specific timing.
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MARK {0006400}}), 5.325);
}

TEST_F (KDF11_InstrTimingTest, DoubleOperandInstructions)
{
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MOV {010000}}), 2.025);  // Mode 0/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MOV {011000}}), 3.450);  // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MOV {012000}}), 3.450);  // Mode 2/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MOV {013000}}), 4.875);  // Mode 3/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MOV {014000}}), 3.750);  // Mode 4/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MOV {015000}}), 5.175);  // Mode 5/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MOV {016000}}), 5.175);  // Mode 6/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MOV {017000}}), 6.600);  // Mode 7/0

    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MOV {010010}}), 4.050);  // Mode 0/1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MOV {010020}}), 4.050);  // Mode 0/2
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MOV {010030}}), 5.175);  // Mode 0/3
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MOV {010040}}), 4.050);  // Mode 0/4
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MOV {010050}}), 5.475);  // Mode 0/5
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MOV {010060}}), 5.475);  // Mode 0/6
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MOV {010070}}), 6.900);  // Mode 0/7

    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MOVB {0111000}}), 3.450);  // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MOVB {0110010}}), 4.050);  // Mode 0/1

    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {CMP {021000}}),   3.450);  // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {CMP {020010}}),   3.750);  // Mode 0/1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {CMPB {0121000}}), 3.450);  // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {CMPB {0120010}}), 3.750);  // Mode 0/1

    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {ADD {061000}}),   3.450);  // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {ADD {060010}}),   4.875);  // Mode 0/1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {SUB {0161000}}),  3.450);  // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {SUB {0160010}}),  4.875);  // Mode 0/1

    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BIT {031000}}),   3.450);  // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BIT {030010}}),   3.750);  // Mode 0/1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BITB {0131000}}), 3.450);  // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BITB {0130010}}), 3.750);  // Mode 0/1

    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BIC {041000}}),   3.450);  // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BIC {040010}}),   4.875);  // Mode 0/1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BICB {0141000}}), 3.450);  // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BICB {0140010}}), 4.875);  // Mode 0/1

    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BIS {051000}}),   3.450);  // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BIS {050010}}),   4.875);  // Mode 0/1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BISB {0151000}}), 3.450);  // Mode 1/0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BISB {0150010}}), 4.875);  // Mode 0/1
}

TEST_F (KDF11_InstrTimingTest, EISInstructions)
{
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {JSR {0004010}}), 4.350);  // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {JSR {0004020}}), 4.650);  // Mode 2
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {JSR {0004030}}), 5.475);  // Mode 3
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {JSR {0004040}}), 4.650);  // Mode 4
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {JSR {0004050}}), 5.775);  // Mode 5
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {JSR {0004060}}), 5.775);  // Mode 6
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {JSR {0004070}}), 7.200);  // Mode 7

    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MUL {070000}}), 34.575);  // Mode 0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {DIV {071000}}), 50.925);  // Mode 0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {ASH {072000}}), 26.100);  // Mode 0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {ASHC {073000}}),47.325);  // Mode 0
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {XOR {074010}}),  4.875);  // Mode 1
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {SOB {077000}}),  2.925);  // Mode 0
}

TEST_F (KDF11_InstrTimingTest, FISInstructions)
{
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {RTS {0000200}}), 3.750);
}

TEST_F (KDF11_InstrTimingTest, BranchInstructions)
{
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BR {0000400}}),  2.025);
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BNE {0001000}}), 2.025);
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BEQ {0001400}}), 2.025);
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BPL {0100000}}), 2.025);
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BMI {0100400}}), 2.025);
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BVC {0102000}}), 2.025);
    // Etc...
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {EMT {0104000}}), 9.525);
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {TRAP {0104400}}),9.525);
}

TEST_F (KDF11_InstrTimingTest, NoOperandInstructions)
{
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {WAIT {0000001}}), 3.375);
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {RTI {0000002}}),  6.225);
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {BPT {0000003}}), 10.500);
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {IOT {0000004}}), 10.500);
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {RESET {0000005}}),2.025);
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {RTT {0000006}}),  7.500);
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {MFPT {0000007}}), 7.500);

    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {CCC {0000240}}), 2.925);
    EXPECT_DOUBLE_EQ (visit (KDF11::Calculate {}, Instruction {SCC {0000260}}), 2.925);
}