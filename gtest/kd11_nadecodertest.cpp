#include "../src/kd11/cpu/lsi11instruction/lsi11instruction.h"
#include "../src/kd11/cpu/kd11_na/kd11_na.h"
#include "../src/kd11/cpu/cpudata.h"
#include "dummycpu/dummycpu.h"

#include <gtest/gtest.h>
#include <string>

using std::unique_ptr;
using std::make_unique;
using std::string;


// Get the mnemonic from the decoded instruction
string getInstrMnemonic (u16 instruction)
{
    KD11_NA kd11_na;
    DummyCpu cpu;
    unique_ptr<LSI11Instruction> instr = kd11_na.decode (&cpu, instruction);
    return instr->mnemonic ();
}

// Verify that decoding of an instruction word will result in the
// creation of the proper instruction. In principle the complete range
// of instruction words (from 000000 till 1777777) is tested.
TEST (KD11_NA_Decoder, Group_00_00_nn)
{
    EXPECT_EQ (getInstrMnemonic (0000000), "HALT");
    EXPECT_EQ (getInstrMnemonic (0000001), "WAIT");
    EXPECT_EQ (getInstrMnemonic (0000007), "Unused");    // till...
    EXPECT_EQ (getInstrMnemonic (0000077), "Unused");
    EXPECT_EQ (getInstrMnemonic (0000200), "RTS");       // till...
    EXPECT_EQ (getInstrMnemonic (0000207), "RTS");
    EXPECT_EQ (getInstrMnemonic (0000210), "Unused");    // till...
    EXPECT_EQ (getInstrMnemonic (0000227), "Unused");
    EXPECT_EQ (getInstrMnemonic (0000240), "CCC");       // NOP is a CCC whitout condition codes
    EXPECT_EQ (getInstrMnemonic (0000241), "CCC");       // till...   
    EXPECT_EQ (getInstrMnemonic (0000257), "CCC");
    EXPECT_EQ (getInstrMnemonic (0000260), "SCC");       // 0260 is a NOP too?
    EXPECT_EQ (getInstrMnemonic (0000261), "SCC");
    EXPECT_EQ (getInstrMnemonic (0000277), "SCC");
    EXPECT_EQ (getInstrMnemonic (0000300), "SWAB");      // till...
    EXPECT_EQ (getInstrMnemonic (0000377), "SWAB");
    EXPECT_EQ (getInstrMnemonic (0000400), "BR");        // till...
    EXPECT_EQ (getInstrMnemonic (0000777), "BR");
    EXPECT_EQ (getInstrMnemonic (0001000), "BNE");       // till...
    EXPECT_EQ (getInstrMnemonic (0001377), "BNE");
    EXPECT_EQ (getInstrMnemonic (0001400), "BEQ");       // till...
    EXPECT_EQ (getInstrMnemonic (0001777), "BEQ");
    EXPECT_EQ (getInstrMnemonic (0002000), "BGE");       // till...
    EXPECT_EQ (getInstrMnemonic (0002377), "BGE");
    EXPECT_EQ (getInstrMnemonic (0002400), "BLT");       // till...
    EXPECT_EQ (getInstrMnemonic (0002777), "BLT");
    EXPECT_EQ (getInstrMnemonic (0003000), "BGT");       // till...
    EXPECT_EQ (getInstrMnemonic (0003377), "BGT");
    EXPECT_EQ (getInstrMnemonic (0003400), "BLE");       // till...
    EXPECT_EQ (getInstrMnemonic (0003777), "BLE");
    EXPECT_EQ (getInstrMnemonic (0004000), "JSR");       // till...
    EXPECT_EQ (getInstrMnemonic (0004777), "JSR");
    EXPECT_EQ (getInstrMnemonic (0005000), "CLR");       // till...
    EXPECT_EQ (getInstrMnemonic (0005077), "CLR");
    EXPECT_EQ (getInstrMnemonic (0005100), "COM");       // till...
    EXPECT_EQ (getInstrMnemonic (0005177), "COM");
    EXPECT_EQ (getInstrMnemonic (0005200), "INC");       // till...
    EXPECT_EQ (getInstrMnemonic (0005277), "INC");
    EXPECT_EQ (getInstrMnemonic (0005300), "DEC");       // till...
    EXPECT_EQ (getInstrMnemonic (0005377), "DEC");
    EXPECT_EQ (getInstrMnemonic (0005400), "NEG");       // till...
    EXPECT_EQ (getInstrMnemonic (0005477), "NEG");
    EXPECT_EQ (getInstrMnemonic (0005500), "ADC");       // till...
    EXPECT_EQ (getInstrMnemonic (0005577), "ADC");
    EXPECT_EQ (getInstrMnemonic (0005600), "SBC");       // till...
    EXPECT_EQ (getInstrMnemonic (0005677), "SBC");
    EXPECT_EQ (getInstrMnemonic (0005700), "TST");       // till...
    EXPECT_EQ (getInstrMnemonic (0005777), "TST");
    EXPECT_EQ (getInstrMnemonic (0006000), "ROR");       // till...
    EXPECT_EQ (getInstrMnemonic (0006077), "ROR");
    EXPECT_EQ (getInstrMnemonic (0006100), "ROL");       // till...
    EXPECT_EQ (getInstrMnemonic (0006177), "ROL");
    EXPECT_EQ (getInstrMnemonic (0006200), "ASR");       // till...
    EXPECT_EQ (getInstrMnemonic (0006277), "ASR");
    EXPECT_EQ (getInstrMnemonic (0006300), "ASL");       // till...
    EXPECT_EQ (getInstrMnemonic (0006377), "ASL");
    EXPECT_EQ (getInstrMnemonic (0006400), "MARK");      // till...
    EXPECT_EQ (getInstrMnemonic (0006477), "MARK");
    EXPECT_EQ (getInstrMnemonic (0006500), "Unused");    // Undefined in the
    EXPECT_EQ (getInstrMnemonic (0006677), "Unused");    // LSI-11 Processor Handbook
    EXPECT_EQ (getInstrMnemonic (0006700), "SXT");       // till...
    EXPECT_EQ (getInstrMnemonic (0006777), "SXT");
    EXPECT_EQ (getInstrMnemonic (0007000), "Unused");    // till...
    EXPECT_EQ (getInstrMnemonic (0007777), "Unused");
    EXPECT_EQ (getInstrMnemonic (0010000), "MOV");       // till...
    EXPECT_EQ (getInstrMnemonic (0017777), "MOV");
    EXPECT_EQ (getInstrMnemonic (0020000), "CMP");       // till...
    EXPECT_EQ (getInstrMnemonic (0027777), "CMP");
    EXPECT_EQ (getInstrMnemonic (0030000), "BIT");       // till...
    EXPECT_EQ (getInstrMnemonic (0037777), "BIT");
    EXPECT_EQ (getInstrMnemonic (0040000), "BIC");       // till...
    EXPECT_EQ (getInstrMnemonic (0047777), "BIC");
    EXPECT_EQ (getInstrMnemonic (0050000), "BIS");       // till...
    EXPECT_EQ (getInstrMnemonic (0057777), "BIS");
    EXPECT_EQ (getInstrMnemonic (0060000), "ADD");       // till...
    EXPECT_EQ (getInstrMnemonic (0067777), "ADD");
    EXPECT_EQ (getInstrMnemonic (0070000), "MUL");       // till...
    EXPECT_EQ (getInstrMnemonic (0070777), "MUL");
    EXPECT_EQ (getInstrMnemonic (0071000), "DIV");       // till...
    EXPECT_EQ (getInstrMnemonic (0071777), "DIV");
    EXPECT_EQ (getInstrMnemonic (0072000), "ASH");       // till...
    EXPECT_EQ (getInstrMnemonic (0072777), "ASH");
    EXPECT_EQ (getInstrMnemonic (0073000), "ASHC");      // till...
    EXPECT_EQ (getInstrMnemonic (0073777), "ASHC");
    EXPECT_EQ (getInstrMnemonic (0074000), "XOR");       // till...
    EXPECT_EQ (getInstrMnemonic (0074777), "XOR");
    EXPECT_EQ (getInstrMnemonic (0075000), "FADD");      // till...
    EXPECT_EQ (getInstrMnemonic (0075007), "FADD");
    EXPECT_EQ (getInstrMnemonic (0075010), "FSUB");      // till...
    EXPECT_EQ (getInstrMnemonic (0075017), "FSUB");
    EXPECT_EQ (getInstrMnemonic (0075020), "FMUL");      // till...
    EXPECT_EQ (getInstrMnemonic (0075027), "FMUL");
    EXPECT_EQ (getInstrMnemonic (0075030), "FDIV");      // till...
    EXPECT_EQ (getInstrMnemonic (0075037), "FDIV");
    EXPECT_EQ (getInstrMnemonic (0075040), "Unused");    // till...
    EXPECT_EQ (getInstrMnemonic (0075777), "Unused");
    EXPECT_EQ (getInstrMnemonic (0076000), "Unused");    // till...
    EXPECT_EQ (getInstrMnemonic (0076777), "Unused");
    EXPECT_EQ (getInstrMnemonic (0077000), "SOB");       // till...
    EXPECT_EQ (getInstrMnemonic (0077777), "SOB");
    EXPECT_EQ (getInstrMnemonic (0100000), "BPL");       // till...
    EXPECT_EQ (getInstrMnemonic (0100377), "BPL");
    EXPECT_EQ (getInstrMnemonic (0100400), "BMI");       // till...
    EXPECT_EQ (getInstrMnemonic (0100777), "BMI");
    EXPECT_EQ (getInstrMnemonic (0101000), "BHI");       // till...
    EXPECT_EQ (getInstrMnemonic (0101377), "BHI");
    EXPECT_EQ (getInstrMnemonic (0101400), "BLOS");      // till...
    EXPECT_EQ (getInstrMnemonic (0101777), "BLOS");
    EXPECT_EQ (getInstrMnemonic (0102000), "BVC");       // till...
    EXPECT_EQ (getInstrMnemonic (0102377), "BVC");
    EXPECT_EQ (getInstrMnemonic (0102400), "BVS");       // till...
    EXPECT_EQ (getInstrMnemonic (0102777), "BVS");
    EXPECT_EQ (getInstrMnemonic (0103000), "BCC");       // till...
    EXPECT_EQ (getInstrMnemonic (0103377), "BCC");
    EXPECT_EQ (getInstrMnemonic (0103400), "BCS");       // till...
    EXPECT_EQ (getInstrMnemonic (0103777), "BCS");
    EXPECT_EQ (getInstrMnemonic (0104000), "EMT");       // till...
    EXPECT_EQ (getInstrMnemonic (0104377), "EMT");
    EXPECT_EQ (getInstrMnemonic (0104400), "TRAP");      // till...
    EXPECT_EQ (getInstrMnemonic (0104777), "TRAP");
    EXPECT_EQ (getInstrMnemonic (0105000), "CLRB");      // till...
    EXPECT_EQ (getInstrMnemonic (0105077), "CLRB");
    EXPECT_EQ (getInstrMnemonic (0105100), "COMB");      // till...
    EXPECT_EQ (getInstrMnemonic (0105177), "COMB");
    EXPECT_EQ (getInstrMnemonic (0105200), "INCB");      // till...
    EXPECT_EQ (getInstrMnemonic (0105277), "INCB");
    EXPECT_EQ (getInstrMnemonic (0105300), "DECB");      // till...
    EXPECT_EQ (getInstrMnemonic (0105377), "DECB");
    EXPECT_EQ (getInstrMnemonic (0105400), "NEGB");      // till...
    EXPECT_EQ (getInstrMnemonic (0105477), "NEGB");
    EXPECT_EQ (getInstrMnemonic (0105500), "ADCB");      // till...
    EXPECT_EQ (getInstrMnemonic (0105577), "ADCB");
    EXPECT_EQ (getInstrMnemonic (0105600), "SBCB");      // till...
    EXPECT_EQ (getInstrMnemonic (0105677), "SBCB");
    EXPECT_EQ (getInstrMnemonic (0105700), "TSTB");      // till...
    EXPECT_EQ (getInstrMnemonic (0105777), "TSTB");
    EXPECT_EQ (getInstrMnemonic (0106000), "RORB");      // till...
    EXPECT_EQ (getInstrMnemonic (0106077), "RORB");
    EXPECT_EQ (getInstrMnemonic (0106100), "ROLB");      // till...
    EXPECT_EQ (getInstrMnemonic (0106177), "ROLB");
    EXPECT_EQ (getInstrMnemonic (0106200), "ASRB");      // till...
    EXPECT_EQ (getInstrMnemonic (0106277), "ASRB");
    EXPECT_EQ (getInstrMnemonic (0106300), "ASLB");      // till...
    EXPECT_EQ (getInstrMnemonic (0106377), "ASLB");
    EXPECT_EQ (getInstrMnemonic (0106400), "MTPS");      // till...
    EXPECT_EQ (getInstrMnemonic (0106477), "MTPS");
    EXPECT_EQ (getInstrMnemonic (0106500), "Unused");    // till...
    EXPECT_EQ (getInstrMnemonic (0106577), "Unused");
    EXPECT_EQ (getInstrMnemonic (0106600), "Unused");    // till...
    EXPECT_EQ (getInstrMnemonic (0106677), "Unused");
    EXPECT_EQ (getInstrMnemonic (0106700), "MFPS");      // till...
    EXPECT_EQ (getInstrMnemonic (0106777), "MFPS");
    EXPECT_EQ (getInstrMnemonic (0107000), "Unused");    // till...
    EXPECT_EQ (getInstrMnemonic (0107777), "Unused");
    EXPECT_EQ (getInstrMnemonic (0110000), "MOVB");      // till...
    EXPECT_EQ (getInstrMnemonic (0117777), "MOVB");
    EXPECT_EQ (getInstrMnemonic (0120000), "CMPB");      // till...
    EXPECT_EQ (getInstrMnemonic (0127777), "CMPB");
    EXPECT_EQ (getInstrMnemonic (0130000), "BITB");      // till...
    EXPECT_EQ (getInstrMnemonic (0137777), "BITB");
    EXPECT_EQ (getInstrMnemonic (0140000), "BICB");      // till...
    EXPECT_EQ (getInstrMnemonic (0147777), "BICB");
    EXPECT_EQ (getInstrMnemonic (0150000), "BISB");      // till...
    EXPECT_EQ (getInstrMnemonic (0157777), "BISB");
    EXPECT_EQ (getInstrMnemonic (0160000), "SUB");       // till...
    EXPECT_EQ (getInstrMnemonic (0167777), "SUB");
    EXPECT_EQ (getInstrMnemonic (0170000), "Unused");    // till...
    EXPECT_EQ (getInstrMnemonic (0177777), "Unused");
}