#ifndef _CALCULATE_H_
#define _CALCULATE_H_

#include "kd/common/addressingmodetimes/addressingmodetimes.h"
#include "../calculate/kd11_na_addressingmodetime.h"
#include "kd/common/instructions/instructions.h"

#include <array>

using std::array;

namespace KD11_NA {

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
	// Definition of source and destination timing, both consisting of
	// a word and byte time per addressing mode.
	AddressingModeTimes<KD11_NA_AddressingModeTime> const SourceTimes
	{
		{0, 0},			// Mode 0
		{1.40, 1.05},	// Mode 1
		{1.40, 1.05},	// Mode 2
		{3.50, 3.15},	// Mode 3
		{2.10, 1.75},	// Mode 4
		{4.20, 3.85},	// Mode 5
		{4.20, 3.85},	// Mode 6
		{6.30, 5.95}	// Mode 7
	};

	AddressingModeTimes<KD11_NA_AddressingModeTime> const DestinationTimes
	{
		{0, 0},			// Mode 0
		{2.10, 1.75},	// Mode 1
		{2.10, 1.75},	// Mode 2
		{4.20, 4.20},	// Mode 3
		{2.80, 2.45},	// Mode 4
		{4.90, 4.90},	// Mode 5
		{4.90, 4.55},	// Mode 6
		{6.65, 7.00}	// Mode 7
	};
};

} // namespace KD11_NA

#endif // !_CALCULATE_H_
