#ifndef _KDF11_CALCULATE_H_
#define _KDF11_CALCULATE_H_

#include "kd/common/addressingmodetimes/addressingmodetimes.h"
#include "kd/kdf11/calculate/kdf11_addressingmodetime.h"
#include "kd/common/instructions/instructions.h"

#include <array>

using std::array;

namespace KDF11 {

class Calculate
{
public:
	// Single operand instructions
    double operator() (CLR const& instr) const;
	double operator() (CLRB const& instr) const;
	double operator() (COM const& instr) const;
	double operator() (COMB const& instr) const;
	double operator() (INC const& instr) const;
	double operator() (INCB const& instr) const;
	double operator() (DEC const& instr) const;
	double operator() (DECB const& instr) const;
	double operator() (NEG const& instr) const;
	double operator() (NEGB const& instr) const;
	double operator() (TST const& instr) const;
	double operator() (TSTB const& instr) const;
	double operator() (ASR const& instr) const;
	double operator() (ASRB const& instr) const;
	double operator() (ASL const& instr) const;
	double operator() (ASLB const& instr) const;
	double operator() (ROR const& instr) const;
	double operator() (RORB const& instr) const;
	double operator() (ROL const& instr) const;
	double operator() (ROLB const& instr) const;
	double operator() (SWAB const& instr) const;
	double operator() (ADC const& instr) const;
	double operator() (ADCB const& instr) const;
	double operator() (SBC const& instr) const;
	double operator() (SBCB const& instr) const;
	double operator() (SXT const& instr) const;
	double operator() (MFPS const& instr) const;
	double operator() (MTPS const& instr) const;
	double operator() (JMP const& instr) const;
	double operator() (MARK const& instr) const;
	double operator() (MFPD const& instr) const;
	double operator() (MFPI const& instr) const;
	double operator() (MTPD const& instr) const;
	double operator() (MTPI const& instr) const;

	// Double operand instructions
	double operator() (MOV const& instr) const;
	double operator() (MOVB const& instr) const;
	double operator() (CMP const& instr) const;
	double operator() (CMPB const& instr) const;
	double operator() (ADD const& instr) const;
	double operator() (SUB const& instr) const;
	double operator() (BIT const& instr) const;
	double operator() (BITB const& instr) const;
	double operator() (BIC const& instr) const;
	double operator() (BICB const& instr) const;
	double operator() (BIS const& instr) const;
	double operator() (BISB const& instr) const;

	// EIS instructions, including JSR and XOR
	double operator() (JSR const& instr) const;
	double operator() (MUL const& instr) const;
	double operator() (DIV const& instr) const;
	double operator() (ASH const& instr) const;
	double operator() (ASHC const& instr) const;
	double operator() (XOR const& instr) const;
	double operator() (SOB const& instr) const;

	// FIS format instructions
	double operator() (RTS const& instr) const;
	double operator() (FADD const& instr) const;
	double operator() (FSUB const& instr) const;
	double operator() (FMUL const& instr) const;
	double operator() (FDIV const& instr) const;

	// Branch instructions
	double operator() (BR const& instr) const;
	double operator() (BNE const& instr) const;
	double operator() (BEQ const& instr) const;
	double operator() (BPL const& instr) const;
	double operator() (BMI const& instr) const;
	double operator() (BVC const& instr) const;
	double operator() (BVS const& instr) const;
	double operator() (BCC const& instr) const;
	double operator() (BCS const& instr) const;
	double operator() (BGE const& instr) const;
	double operator() (BLT const& instr) const;
	double operator() (BGT const& instr) const;
	double operator() (BLE const& instr) const;
	double operator() (BHI const& instr) const;
	double operator() (BLOS const& instr) const;
	double operator() (EMT const& instr) const;
	double operator() (TRAP const& instr) const;

	// No-operand instructions
	double operator() (HALT const& instr) const;
	double operator() (WAIT const& instr) const;
	double operator() (RTI const& instr) const;
	double operator() (BPT const& instr) const;
	double operator() (IOT const& instr) const;
	double operator() (RESET const& instr) const;
	double operator() (RTT const& instr) const;
	double operator() (CCC const& instr) const;
	double operator() (SCC const& instr) const;
    double operator() (MFPT const& instr) const;
	double operator() (Unused const& instr) const;

private:
    AddressingModeTimes<KDF11_AddressingModeTime> const sourceB4Group1Times
    {
        {0.0, 0.0},
        {1.425, 1.200},
        {1.425, 1.200},
        {2.850, 2.400},
        {1.725, 1.500},
        {3.150, 2.700},
        {3.150, 2.700},
        {4.575, 3.900}
    };

    AddressingModeTimes<KDF11_AddressingModeTime> const sourceB4Group2Times
    {
        {1.275, 1.275},
        {1.725, 1.450},
        {1.725, 1.450},
        {2.850, 2.400},
        {1.725, 1.500},
        {3.150, 2.700},
        {3.150, 2.700},
        {4.575, 3.900}
    };

    AddressingModeTimes<KDF11_AddressingModeTime> const destinationB5Group1Times
    {
        {0.0, 0.0},
        {2.025, 1.800},
        {2.025, 1.800},
        {3.150, 2.700},
        {2.025, 1.950},
        {3.450, 3.000},
        {3.450, 3.000},
        {4.875, 4.500}
    };

    AddressingModeTimes<KDF11_AddressingModeTime> const destinationB5Group2Times
    {
        {0.0, 0.0},
        {1.725, 1.500},
        {1.725, 1.500},
        {2.850, 2.400},
        {1.725, 1.500},
        {3.150, 2.700},
        {3.150, 2.700},
        {4.575, 3.900}
    };

    AddressingModeTimes<KDF11_AddressingModeTime> const destinationB5Group3Times
    {
        {0.0, 0.0},
        {2.850, 2.625},
        {2.850, 2.625},
        {4.275, 3.825},
        {2.850, 2.625},
        {4.575, 4.125},
        {4.575, 4.125},
        {6.000, 5.325}
    };

    // Basic (Fetch and Execute) Times as defined in Table B-6.
    // For the SOB instruction only the No Branch time is recorded in this
    // table as a SOB instruction in most of the cases will not branch.
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group1Times {{2.025, 1.800}};     // MOV, etc.
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group2Times {{3.600, 3.375}};     // MTPS
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group3Times {{4.050, 3.600}};     // MFPI, MFPD
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group4Times {{4.725, 2.275}};     // MTPI, MTPD
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group5Times {{2.925, 2.700}};     // SOB (BRANCH)
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group6Times {{2.025, 1.800}};     // ALL BRANCHES
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group7Times {{2.925, 2.700}};     // CLN, etc.
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group8Times {{3.750, 3.300}};     // RTS
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group9Times {{5.325, 4.875}};     // MARK
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group10Times {{6.225, 5.550}};     // RTI
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group11Times {{7.500, 6.825}};     // RTT
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group12Times {{10.500, 9.375}};     // IOT, BPT
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group13Times {{9.525, 8.850}};     // EMT, TRAP
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group14Times {{3.375, 3.150}};     // WAIT
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group15Times {{33.300, 33.075}};    // MUL
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group16Times {{49.650, 49.425}};    // DIV
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group17Times {{24.825, 24.600}};    // ASH
    AddressingModeTimes<KDF11_AddressingModeTime> const basicB6Group18Times {{46.050, 45.825}};    // ASHC

    AddressingModeTimes<KDF11_AddressingModeTime> const basicB7JMPTimes
    {
        {0.0, 0.0},
        {2.325, 2.100},
        {2.625, 2.400},
        {3.450, 3.000},
        {2.625, 2.400},
        {3.750, 3.300},
        {3.750, 3.300},
        {5.175, 4.500}
    };

    AddressingModeTimes<KDF11_AddressingModeTime> const basicB7JSRTimes
    {
        {0.0, 0.0},
        {4.350, 3.900},
        {4.650, 4.200},
        {5.475, 4.800},
        {4.650, 4.200},
        {5.775, 5.100},
        {5.775, 5.100},
        {7.200, 6.300}
    };

    // The KDF11-B manual doesn't state the instruction timing for the HALT
    // and RESET instructions but that timing isn't very relevant for us anyway.
    // For the HALT instruction we don't take any instruction time into account
    // and for the RESET instruction we'll use an average instruction time.
    AddressingModeTimes<KDF11_AddressingModeTime> const basicHALTTimes {{0.0, 0.0}};
    AddressingModeTimes<KDF11_AddressingModeTime> const basicRESETTimes {{5.0, 4.750}};
};

} // namespace KDF11

#endif // !_KDF11_CALCULATE_H_
