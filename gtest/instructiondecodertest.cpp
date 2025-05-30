#include "kd/common/instructiondecoder/instructiondecoder.h"
#include "kd/common/instructions/instructions.h"

#include <gtest/gtest.h>

using std::holds_alternative;


// Verify that decoding of an instruction word will result in the
// creation of the proper instruction. In principle the complete range
// of instruction words (from 000000 till 1777777) is tested.
TEST (InstructionDecoder, Group_00_00_nn)
{
    InstructionDecoder decoder;

    EXPECT_TRUE (holds_alternative<HALT> (decoder.decode (000000)));
    EXPECT_TRUE (holds_alternative<WAIT> (decoder.decode (0000001)));
    EXPECT_TRUE (holds_alternative<MFPT> (decoder.decode (0000007))); 
    EXPECT_TRUE (holds_alternative<Unused> (decoder.decode (0000010)));    // till...
    EXPECT_TRUE (holds_alternative<Unused> (decoder.decode (0000077)));
    EXPECT_TRUE (holds_alternative<RTS> (decoder.decode (0000200)));       // till...
    EXPECT_TRUE (holds_alternative<RTS> (decoder.decode (0000207)));
    EXPECT_TRUE (holds_alternative<Unused> (decoder.decode (0000210)));    // till...
    EXPECT_TRUE (holds_alternative<Unused> (decoder.decode (0000227)));
    EXPECT_TRUE (holds_alternative<CCC> (decoder.decode (0000240)));       // NOP is a CCC whitout condition codes
    EXPECT_TRUE (holds_alternative<CCC> (decoder.decode (0000241)));       // till...   
    EXPECT_TRUE (holds_alternative<CCC> (decoder.decode (0000257)));
    EXPECT_TRUE (holds_alternative<SCC> (decoder.decode (0000260)));       // 0260 is a NOP too
    EXPECT_TRUE (holds_alternative<SCC> (decoder.decode (0000261)));
    EXPECT_TRUE (holds_alternative<SCC> (decoder.decode (0000277)));
    EXPECT_TRUE (holds_alternative<SWAB> (decoder.decode (0000300)));      // till...
    EXPECT_TRUE (holds_alternative<SWAB> (decoder.decode (0000377)));
    EXPECT_TRUE (holds_alternative<BR> (decoder.decode (0000400)));        // till...
    EXPECT_TRUE (holds_alternative<BR> (decoder.decode (0000777)));
    EXPECT_TRUE (holds_alternative<BNE> (decoder.decode (0001000)));       // till...
    EXPECT_TRUE (holds_alternative<BNE> (decoder.decode (0001377)));
    EXPECT_TRUE (holds_alternative<BEQ> (decoder.decode (0001400)));       // till...
    EXPECT_TRUE (holds_alternative<BEQ> (decoder.decode (0001777)));
    EXPECT_TRUE (holds_alternative<BGE> (decoder.decode (0002000)));       // till...
    EXPECT_TRUE (holds_alternative<BGE> (decoder.decode (0002377)));
    EXPECT_TRUE (holds_alternative<BLT> (decoder.decode (0002400)));       // till...
    EXPECT_TRUE (holds_alternative<BLT> (decoder.decode (0002777)));
    EXPECT_TRUE (holds_alternative<BGT> (decoder.decode (0003000)));       // till...
    EXPECT_TRUE (holds_alternative<BGT> (decoder.decode (0003377)));
    EXPECT_TRUE (holds_alternative<BLE> (decoder.decode (0003400)));       // till...
    EXPECT_TRUE (holds_alternative<BLE> (decoder.decode (0003777)));
    EXPECT_TRUE (holds_alternative<JSR> (decoder.decode (0004000)));       // till...
    EXPECT_TRUE (holds_alternative<JSR> (decoder.decode (0004777)));
    EXPECT_TRUE (holds_alternative<CLR> (decoder.decode (0005000)));       // till...
    EXPECT_TRUE (holds_alternative<CLR> (decoder.decode (0005077)));
    EXPECT_TRUE (holds_alternative<COM> (decoder.decode (0005100)));       // till...
    EXPECT_TRUE (holds_alternative<COM> (decoder.decode (0005177)));
    EXPECT_TRUE (holds_alternative<INC> (decoder.decode (0005200)));       // till...
    EXPECT_TRUE (holds_alternative<INC> (decoder.decode (0005277)));
    EXPECT_TRUE (holds_alternative<DEC> (decoder.decode (0005300)));       // till...
    EXPECT_TRUE (holds_alternative<DEC> (decoder.decode (0005377)));
    EXPECT_TRUE (holds_alternative<NEG> (decoder.decode (0005400)));       // till...
    EXPECT_TRUE (holds_alternative<NEG> (decoder.decode (0005477)));
    EXPECT_TRUE (holds_alternative<ADC> (decoder.decode (0005500)));       // till...
    EXPECT_TRUE (holds_alternative<ADC> (decoder.decode (0005577)));
    EXPECT_TRUE (holds_alternative<SBC> (decoder.decode (0005600)));       // till...
    EXPECT_TRUE (holds_alternative<SBC> (decoder.decode (0005677)));
    EXPECT_TRUE (holds_alternative<TST> (decoder.decode (0005700)));       // till...
    EXPECT_TRUE (holds_alternative<TST> (decoder.decode (0005777)));
    EXPECT_TRUE (holds_alternative<ROR> (decoder.decode (0006000)));       // till...
    EXPECT_TRUE (holds_alternative<ROR> (decoder.decode (0006077)));
    EXPECT_TRUE (holds_alternative<ROL> (decoder.decode (0006100)));       // till...
    EXPECT_TRUE (holds_alternative<ROL> (decoder.decode (0006177)));
    EXPECT_TRUE (holds_alternative<ASR> (decoder.decode (0006200)));       // till...
    EXPECT_TRUE (holds_alternative<ASR> (decoder.decode (0006277)));
    EXPECT_TRUE (holds_alternative<ASL> (decoder.decode (0006300)));       // till...
    EXPECT_TRUE (holds_alternative<ASL> (decoder.decode (0006377)));
    EXPECT_TRUE (holds_alternative<MARK> (decoder.decode (0006400)));      // till...
    EXPECT_TRUE (holds_alternative<MARK> (decoder.decode (0006477)));
    EXPECT_TRUE (holds_alternative<MFPD> (decoder.decode (0006500)));      // till..
    EXPECT_TRUE (holds_alternative<MFPD> (decoder.decode (0006577)));
    EXPECT_TRUE (holds_alternative<MTPI> (decoder.decode (0006600)));      // till...
    EXPECT_TRUE (holds_alternative<MTPI> (decoder.decode (0006677)));
    EXPECT_TRUE (holds_alternative<SXT> (decoder.decode (0006700)));       // till...
    EXPECT_TRUE (holds_alternative<SXT> (decoder.decode (0006777)));
    EXPECT_TRUE (holds_alternative<Unused> (decoder.decode (0007000)));    // till...
    EXPECT_TRUE (holds_alternative<Unused> (decoder.decode (0007777)));
    EXPECT_TRUE (holds_alternative<MOV> (decoder.decode (0010000)));       // till...
    EXPECT_TRUE (holds_alternative<MOV> (decoder.decode (0017777)));
    EXPECT_TRUE (holds_alternative<CMP> (decoder.decode (0020000)));       // till...
    EXPECT_TRUE (holds_alternative<CMP> (decoder.decode (0027777)));
    EXPECT_TRUE (holds_alternative<BIT> (decoder.decode (0030000)));       // till...
    EXPECT_TRUE (holds_alternative<BIT> (decoder.decode (0037777)));
    EXPECT_TRUE (holds_alternative<BIC> (decoder.decode (0040000)));       // till...
    EXPECT_TRUE (holds_alternative<BIC> (decoder.decode (0047777)));
    EXPECT_TRUE (holds_alternative<BIS> (decoder.decode (0050000)));       // till...
    EXPECT_TRUE (holds_alternative<BIS> (decoder.decode (0057777)));
    EXPECT_TRUE (holds_alternative<ADD> (decoder.decode (0060000)));       // till...
    EXPECT_TRUE (holds_alternative<ADD> (decoder.decode (0067777)));
    EXPECT_TRUE (holds_alternative<MUL> (decoder.decode (0070000)));       // till...
    EXPECT_TRUE (holds_alternative<MUL> (decoder.decode (0070777)));
    EXPECT_TRUE (holds_alternative<DIV> (decoder.decode (0071000)));       // till...
    EXPECT_TRUE (holds_alternative<DIV> (decoder.decode (0071777)));
    EXPECT_TRUE (holds_alternative<ASH> (decoder.decode (0072000)));       // till...
    EXPECT_TRUE (holds_alternative<ASH> (decoder.decode (0072777)));
    EXPECT_TRUE (holds_alternative<ASHC> (decoder.decode (0073000)));      // till...
    EXPECT_TRUE (holds_alternative<ASHC> (decoder.decode (0073777)));
    EXPECT_TRUE (holds_alternative<XOR> (decoder.decode (0074000)));       // till...
    EXPECT_TRUE (holds_alternative<XOR> (decoder.decode (0074777)));
    EXPECT_TRUE (holds_alternative<FADD> (decoder.decode (0075000)));      // till...
    EXPECT_TRUE (holds_alternative<FADD> (decoder.decode (0075007)));
    EXPECT_TRUE (holds_alternative<FSUB> (decoder.decode (0075010)));      // till...
    EXPECT_TRUE (holds_alternative<FSUB> (decoder.decode (0075017)));
    EXPECT_TRUE (holds_alternative<FMUL> (decoder.decode (0075020)));      // till...
    EXPECT_TRUE (holds_alternative<FMUL> (decoder.decode (0075027)));
    EXPECT_TRUE (holds_alternative<FDIV> (decoder.decode (0075030)));      // till...
    EXPECT_TRUE (holds_alternative<FDIV> (decoder.decode (0075037)));
    EXPECT_TRUE (holds_alternative<Unused> (decoder.decode (0075040)));    // till...
    EXPECT_TRUE (holds_alternative<Unused> (decoder.decode (0075777)));
    EXPECT_TRUE (holds_alternative<Unused> (decoder.decode (0076000)));    // till...
    EXPECT_TRUE (holds_alternative<Unused> (decoder.decode (0076777)));
    EXPECT_TRUE (holds_alternative<SOB> (decoder.decode (0077000)));       // till...
    EXPECT_TRUE (holds_alternative<SOB> (decoder.decode (0077777)));
    EXPECT_TRUE (holds_alternative<BPL> (decoder.decode (0100000)));       // till...
    EXPECT_TRUE (holds_alternative<BPL> (decoder.decode (0100377)));
    EXPECT_TRUE (holds_alternative<BMI> (decoder.decode (0100400)));       // till...
    EXPECT_TRUE (holds_alternative<BMI> (decoder.decode (0100777)));
    EXPECT_TRUE (holds_alternative<BHI> (decoder.decode (0101000)));       // till...
    EXPECT_TRUE (holds_alternative<BHI> (decoder.decode (0101377)));
    EXPECT_TRUE (holds_alternative<BLOS> (decoder.decode (0101400)));      // till...
    EXPECT_TRUE (holds_alternative<BLOS> (decoder.decode (0101777)));
    EXPECT_TRUE (holds_alternative<BVC> (decoder.decode (0102000)));       // till...
    EXPECT_TRUE (holds_alternative<BVC> (decoder.decode (0102377)));
    EXPECT_TRUE (holds_alternative<BVS> (decoder.decode (0102400)));       // till...
    EXPECT_TRUE (holds_alternative<BVS> (decoder.decode (0102777)));
    EXPECT_TRUE (holds_alternative<BCC> (decoder.decode (0103000)));       // till...
    EXPECT_TRUE (holds_alternative<BCC> (decoder.decode (0103377)));
    EXPECT_TRUE (holds_alternative<BCS> (decoder.decode (0103400)));       // till...
    EXPECT_TRUE (holds_alternative<BCS> (decoder.decode (0103777)));
    EXPECT_TRUE (holds_alternative<EMT> (decoder.decode (0104000)));       // till...
    EXPECT_TRUE (holds_alternative<EMT> (decoder.decode (0104377)));
    EXPECT_TRUE (holds_alternative<TRAP> (decoder.decode (0104400)));      // till...
    EXPECT_TRUE (holds_alternative<TRAP> (decoder.decode (0104777)));
    EXPECT_TRUE (holds_alternative<CLRB> (decoder.decode (0105000)));      // till...
    EXPECT_TRUE (holds_alternative<CLRB> (decoder.decode (0105077)));
    EXPECT_TRUE (holds_alternative<COMB> (decoder.decode (0105100)));      // till...
    EXPECT_TRUE (holds_alternative<COMB> (decoder.decode (0105177)));
    EXPECT_TRUE (holds_alternative<INCB> (decoder.decode (0105200)));      // till...
    EXPECT_TRUE (holds_alternative<INCB> (decoder.decode (0105277)));
    EXPECT_TRUE (holds_alternative<DECB> (decoder.decode (0105300)));      // till...
    EXPECT_TRUE (holds_alternative<DECB> (decoder.decode (0105377)));
    EXPECT_TRUE (holds_alternative<NEGB> (decoder.decode (0105400)));      // till...
    EXPECT_TRUE (holds_alternative<NEGB> (decoder.decode (0105477)));
    EXPECT_TRUE (holds_alternative<ADCB> (decoder.decode (0105500)));      // till...
    EXPECT_TRUE (holds_alternative<ADCB> (decoder.decode (0105577)));
    EXPECT_TRUE (holds_alternative<SBCB> (decoder.decode (0105600)));      // till...
    EXPECT_TRUE (holds_alternative<SBCB> (decoder.decode (0105677)));
    EXPECT_TRUE (holds_alternative<TSTB> (decoder.decode (0105700)));      // till...
    EXPECT_TRUE (holds_alternative<TSTB> (decoder.decode (0105777)));
    EXPECT_TRUE (holds_alternative<RORB> (decoder.decode (0106000)));      // till...
    EXPECT_TRUE (holds_alternative<RORB> (decoder.decode (0106077)));
    EXPECT_TRUE (holds_alternative<ROLB> (decoder.decode (0106100)));      // till...
    EXPECT_TRUE (holds_alternative<ROLB> (decoder.decode (0106177)));
    EXPECT_TRUE (holds_alternative<ASRB> (decoder.decode (0106200)));      // till...
    EXPECT_TRUE (holds_alternative<ASRB> (decoder.decode (0106277)));
    EXPECT_TRUE (holds_alternative<ASLB> (decoder.decode (0106300)));      // till...
    EXPECT_TRUE (holds_alternative<ASLB> (decoder.decode (0106377)));
    EXPECT_TRUE (holds_alternative<MTPS> (decoder.decode (0106400)));      // till...
    EXPECT_TRUE (holds_alternative<MTPS> (decoder.decode (0106477)));
    EXPECT_TRUE (holds_alternative<MFPI> (decoder.decode (0106500)));      // till...
    EXPECT_TRUE (holds_alternative<MFPI> (decoder.decode (0106577)));
    EXPECT_TRUE (holds_alternative<MTPD> (decoder.decode (0106600)));      // till...
    EXPECT_TRUE (holds_alternative<MTPD> (decoder.decode (0106677)));
    EXPECT_TRUE (holds_alternative<MFPS> (decoder.decode (0106700)));      // till...
    EXPECT_TRUE (holds_alternative<MFPS> (decoder.decode (0106777)));
    EXPECT_TRUE (holds_alternative<Unused> (decoder.decode (0107000)));    // till...
    EXPECT_TRUE (holds_alternative<Unused> (decoder.decode (0107777)));
    EXPECT_TRUE (holds_alternative<MOVB> (decoder.decode (0110000)));      // till...
    EXPECT_TRUE (holds_alternative<MOVB> (decoder.decode (0117777)));
    EXPECT_TRUE (holds_alternative<CMPB> (decoder.decode (0120000)));      // till...
    EXPECT_TRUE (holds_alternative<CMPB> (decoder.decode (0127777)));
    EXPECT_TRUE (holds_alternative<BITB> (decoder.decode (0130000)));      // till...
    EXPECT_TRUE (holds_alternative<BITB> (decoder.decode (0137777)));
    EXPECT_TRUE (holds_alternative<BICB> (decoder.decode (0140000)));      // till...
    EXPECT_TRUE (holds_alternative<BICB> (decoder.decode (0147777)));
    EXPECT_TRUE (holds_alternative<BISB> (decoder.decode (0150000)));      // till...
    EXPECT_TRUE (holds_alternative<BISB> (decoder.decode (0157777)));
    EXPECT_TRUE (holds_alternative<SUB> (decoder.decode (0160000)));       // till...
    EXPECT_TRUE (holds_alternative<SUB> (decoder.decode (0167777)));
    EXPECT_TRUE (holds_alternative<Unused> (decoder.decode (0170000)));    // till...
    EXPECT_TRUE (holds_alternative<Unused> (decoder.decode (0177777)));
}