#ifndef _KA11_CALCULATOR_H_
#define _KA11_CALCULATOR_H_

#include "proc/common/addressingmodetimes/addressingmodetimes.h"
#include "proc/kd/common/instructions/instructions.h"

// This class calculates the execution time of instructions for the
// KA11. The instruction consits of a basic time per instruction plus
// the time needed to access the source and/or destination operands.
// All values are in microseconds (µs).
// 
// Source: PDP-11 Handbook 1969, appendices A and B.
//
class KA11Calculator
{
public:
	// Single operand instructions
	double operator() (CLR const& instr) const 	{ return calcSingleOperand<2.3> (instr); }
	double operator() (CLRB const& instr) const { return calcSingleOperand<2.3> (instr); }
	double operator() (COM const& instr) const  { return calcSingleOperand<2.3> (instr); }
	double operator() (COMB const& instr) const { return calcSingleOperand<2.3> (instr); }
	double operator() (INC const& instr) const  { return calcSingleOperand<2.3> (instr); }
	double operator() (INCB const& instr) const { return calcSingleOperand<2.3> (instr); }
	double operator() (DEC const& instr) const  { return calcSingleOperand<2.3> (instr); }
	double operator() (DECB const& instr) const { return calcSingleOperand<2.3> (instr); }
	double operator() (NEG const& instr) const  { return calcSingleOperand<2.3> (instr); }
	double operator() (NEGB const& instr) const { return calcSingleOperand<2.3> (instr); }
	double operator() (TST const& instr) const  { return calcSingleOperand<2.3> (instr); }
	double operator() (TSTB const& instr) const { return calcSingleOperand<2.3> (instr); }
	double operator() (ASR const& instr) const  { return calcSingleOperand<2.3> (instr); }
	double operator() (ASRB const& instr) const { return calcSingleOperand<2.3> (instr); }
	double operator() (ASL const& instr) const  { return calcSingleOperand<2.3> (instr);}
	double operator() (ASLB const& instr) const { return calcSingleOperand<2.3> (instr); }
	double operator() (ROR const& instr) const  { return calcSingleOperand<2.3> (instr); }
	double operator() (RORB const& instr) const { return calcSingleOperand<2.3> (instr);}
	double operator() (ROL const& instr) const  { return calcSingleOperand<2.3> (instr); }
	double operator() (ROLB const& instr) const { return calcSingleOperand<2.3> (instr); }
	double operator() (SWAB const& instr) const { return calcSingleOperand<2.3> (instr); }
	double operator() (ADC const& instr) const  { return calcSingleOperand<2.3> (instr); }
	double operator() (ADCB const& instr) const { return calcSingleOperand<2.3> (instr); }
	double operator() (SBC const& instr) const  { return calcSingleOperand<2.3> (instr); }
	double operator() (SBCB const& instr) const { return calcSingleOperand<2.3> (instr); }
	double operator() (SXT const& instr) const  { return calcUnsupportedInstruction (instr); }
	double operator() (MFPS const& instr) const { return calcUnsupportedInstruction (instr); }
	double operator() (MTPS const& instr) const { return calcUnsupportedInstruction (instr); }
	double operator() (JMP const& instr) const  { return calcSingleOperand<2.3> (instr); }
	double operator() (MARK const& instr) const { return 11.55; }
	double operator() (MFPD const& instr) const { return calcUnsupportedInstruction (instr); }
	double operator() (MFPI const& instr) const { return calcUnsupportedInstruction (instr); }
	double operator() (MTPD const& instr) const { return calcUnsupportedInstruction (instr); }
	double operator() (MTPI const& instr) const { return calcUnsupportedInstruction (instr); }

	// Double operand instructions
	double operator() (MOV const& instr) const  { return calcDoubleOperand<2.3> (instr); }
	double operator() (MOVB const& instr) const { return calcDoubleOperand<2.3> (instr); }
	double operator() (CMP const& instr) const  { return calcDoubleOperand<2.3> (instr); }
	double operator() (CMPB const& instr) const { return calcDoubleOperand<2.3> (instr); }
	double operator() (ADD const& instr) const  { return calcDoubleOperand<2.3> (instr); }
	double operator() (SUB const& instr) const  { return calcDoubleOperand<2.3> (instr); }
	double operator() (BIT const& instr) const  { return calcDoubleOperand<2.9> (instr); }
	double operator() (BITB const& instr) const { return calcDoubleOperand<2.9> (instr); }
	double operator() (BIC const& instr) const  { return calcDoubleOperand<2.9> (instr); }
	double operator() (BICB const& instr) const { return calcDoubleOperand<2.9> (instr); }
	double operator() (BIS const& instr) const  { return calcDoubleOperand<2.3> (instr); }
	double operator() (BISB const& instr) const { return calcDoubleOperand<2.3> (instr); }

	// EIS instructions, including JSR and XOR
	double operator() (JSR const& instr) const  { return 2.3 + destinationTime_[instr.getOperandMode ()]; }
	double operator() (MUL const& instr) const  { return calcUnsupportedInstruction (instr); }
	double operator() (DIV const& instr) const  { return calcUnsupportedInstruction (instr); }
	double operator() (ASH const& instr) const  { return calcUnsupportedInstruction (instr); }
	double operator() (ASHC const& instr) const { return calcUnsupportedInstruction (instr); }
	double operator() (XOR const& instr) const  { return calcUnsupportedInstruction (instr); }
	double operator() (SOB const& instr) const  { return calcUnsupportedInstruction (instr); }

	// FIS format instructions
	double operator() (RTS const& instr) const  { return 3.5; }
	double operator() (FADD const& instr) const { return calcUnsupportedInstruction (instr); }
	double operator() (FSUB const& instr) const { return calcUnsupportedInstruction (instr); }
	double operator() (FMUL const& instr) const { return calcUnsupportedInstruction (instr); }
	double operator() (FDIV const& instr) const { return calcUnsupportedInstruction (instr); }

	// Branch instructions
	double operator() (BR const& instr) const   { return 2.6; }
	double operator() (BNE const& instr) const  { return 2.6; }
	double operator() (BEQ const& instr) const  { return 2.6; }
	double operator() (BPL const& instr) const  { return 2.6; }
	double operator() (BMI const& instr) const  { return 2.6; }
	double operator() (BVC const& instr) const  { return 2.6; }
	double operator() (BVS const& instr) const  { return 2.6; }
	double operator() (BCC const& instr) const  { return 2.6; }
	double operator() (BCS const& instr) const  { return 2.6; }
	double operator() (BGE const& instr) const  { return 2.6; }
	double operator() (BLT const& instr) const  { return 2.6; }
	double operator() (BGT const& instr) const  { return 2.6; }
	double operator() (BLE const& instr) const  { return 3.50; }
	double operator() (BHI const& instr) const  { return 2.6; }
	double operator() (BLOS const& instr) const { return 2.6; }
	double operator() (EMT const& instr) const  { return 8.9; }
	double operator() (TRAP const& instr) const { return 8.9; }


	// No-operand instructions
	double operator() (HALT const& instr) const   { return 1.8; }
	double operator() (WAIT const& instr) const   { return 1.8; }
	double operator() (RTI const& instr) const    { return 4.8; }
	double operator() (BPT const& instr) const    { return calcUnsupportedInstruction (instr); }
	double operator() (IOT const& instr) const    { return 8.9; }
	double operator() (RESET const& instr) const  { return 20'000; }
	double operator() (RTT const& instr) const    { return calcUnsupportedInstruction (instr); }
	double operator() (CCC const& instr) const    { return calcUnsupportedInstruction (instr); }
	double operator() (SCC const& instr) const    { return calcUnsupportedInstruction (instr); }
	double operator() (MFPT const& instr) const   { return calcUnsupportedInstruction (instr); }
	double operator() (Unused const& instr) const { return calcUnsupportedInstruction (instr); }

private:
	// Definition of source and destination timing per addressing mode
	AddressingModeTimes<double> const sourceTime_
	{
		0,		// Mode 0
		1.5,	// Mode 1
		1.5,	// Mode 2
		2.7,	// Mode 3
		1.5,	// Mode 4
		2.7,	// Mode 5
		2.7,	// Mode 6
		3.9		// Mode 7
	};

	AddressingModeTimes<double> const destinationTime_
	{
		0,		// Mode 0
		1.4,	// Mode 1
		1.4,	// Mode 2
		2.6,	// Mode 3
		1.4,	// Mode 4
		2.6,	// Mode 5
		2.6,	// Mode 6
		3.8		// Mode 7
	};

	template<double BaseTime, typename Instr>
	double calcSingleOperand (Instr const& instr) const
	{
		return BaseTime
			+ sourceTime_[instr.getMode ()]
			+ destinationTime_[instr.getMode ()];
	}

	template<double BaseTime, typename Instr>
	double calcDoubleOperand (Instr const& instr) const
	{
		return BaseTime
			+ sourceTime_[instr.getSourceMode ()]
			+ destinationTime_[instr.getDestinationMode ()];
	}

	template<typename Instr>
	double calcUnsupportedInstruction (Instr const& instr) const
	{
		return 0;
	}
};

#endif // !_KA11_CALCULATOR_H_
