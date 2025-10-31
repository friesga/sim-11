#include "calculate.h"
#include "calculate.h"

#include <iostream>

// Logic for calculating the execution time of the instructions.
// The AddressinModeTimes arrays are defined static to avoid creating
// the arrays at every instruction execution.
//
// Single operand instructions
//
double KD11_NA::Calculate::operator() (CLR const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const CLR_Times {{3.85}, {4.20}};
    return CLR_Times [instr.getMode ()].word_ +
        DestinationTimes[instr.getMode ()].word_;
}

double KD11_NA::Calculate::operator() (CLRB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const CLRB_Times {{3.85}, {4.20}};
    return CLRB_Times[instr.getMode ()].byte_ +
        DestinationTimes[instr.getMode ()].byte_;
}

double KD11_NA::Calculate::operator() (COM const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const COM_Times {{4.20}, {4.55}};
    return COM_Times[instr.getMode ()].word_ +
        DestinationTimes[instr.getMode ()].word_;
}

double KD11_NA::Calculate::operator() (COMB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const COMB_Times {{3.85}, {4.20}};
    return COMB_Times[instr.getMode ()].byte_ +
        DestinationTimes[instr.getMode ()].byte_;
}

double KD11_NA::Calculate::operator() (INC const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const INC_Times {{4.20}, {4.90}};
    return INC_Times[instr.getMode ()].word_ +
        DestinationTimes[instr.getMode ()].word_;
}

double KD11_NA::Calculate::operator() (INCB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const INCB_Times {{3.85}, {4.55}};
    return INCB_Times[instr.getMode ()].byte_ +
        DestinationTimes[instr.getMode ()].byte_;
}

double KD11_NA::Calculate::operator() (DEC const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const DEC_Times {{4.20}, {4.90}};
    return DEC_Times[instr.getMode ()].word_ +
        DestinationTimes[instr.getMode ()].word_;
}

double KD11_NA::Calculate::operator() (DECB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const DECB_Times {{3.85}, {4.55}};
    return DECB_Times[instr.getMode ()].byte_ +
        DestinationTimes[instr.getMode ()].byte_;
}

double KD11_NA::Calculate::operator() (NEG const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const NEG_Times {{4.20}, {4.55}};
    return NEG_Times[instr.getMode ()].word_ +
        DestinationTimes[instr.getMode ()].word_;
}

double KD11_NA::Calculate::operator() (NEGB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const NEGB_Times {{3.85}, {4.20}};
    return NEGB_Times[instr.getMode ()].byte_ +
        DestinationTimes[instr.getMode ()].byte_;
}

double KD11_NA::Calculate::operator() (TST const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const TST_Times {{4.20}, {3.85}};
    return TST_Times[instr.getMode ()].word_ +
        DestinationTimes[instr.getMode ()].word_;
}

double KD11_NA::Calculate::operator() (TSTB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const TSTB_Times {{3.85}, {3.50}};
    return TSTB_Times[instr.getMode ()].byte_ +
        DestinationTimes[instr.getMode ()].byte_;
}

double KD11_NA::Calculate::operator() (ASR const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const ASR_Times {{5.60}, {6.30}};
    return ASR_Times[instr.getMode ()].word_ +
        DestinationTimes[instr.getMode ()].word_;
}

double KD11_NA::Calculate::operator() (ASRB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const ASRB_Times {{4.55}, {5.95}};
    return ASRB_Times[instr.getMode ()].byte_ +
        DestinationTimes[instr.getMode ()].byte_;
}

double KD11_NA::Calculate::operator() (ASL const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const ASL_Times {{3.85}, {4.55}};
    return ASL_Times[instr.getMode ()].word_ +
        DestinationTimes[instr.getMode ()].word_;
}

double KD11_NA::Calculate::operator() (ASLB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const ASLB_Times {{3.85}, {4.20}};
    return ASLB_Times[instr.getMode ()].byte_ +
        DestinationTimes[instr.getMode ()].byte_;
}

double KD11_NA::Calculate::operator() (ROR const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const ROR_Times {{5.25}, {5.95}};
    return ROR_Times[instr.getMode ()].word_ +
        DestinationTimes[instr.getMode ()].word_;
}

double KD11_NA::Calculate::operator() (RORB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const RORB_Times {{4.20}, {4.90}};
    return RORB_Times[instr.getMode ()].byte_ +
        DestinationTimes[instr.getMode ()].byte_;
}

double KD11_NA::Calculate::operator() (ROL const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const ROL_Times {{3.85}, {4.55}};
    return ROL_Times[instr.getMode ()].word_ +
        DestinationTimes[instr.getMode ()].word_;
}

double KD11_NA::Calculate::operator() (ROLB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const ROLB_Times {{3.85}, {4.20}};
    return ROLB_Times[instr.getMode ()].byte_ +
        DestinationTimes[instr.getMode ()].byte_;
}

double KD11_NA::Calculate::operator() (SWAB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const SWAB_Times {{4.20}, {3.85}};
    return SWAB_Times[instr.getMode ()].word_ +
        DestinationTimes[instr.getMode ()].word_;
}

double KD11_NA::Calculate::operator() (ADC const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const ADC_Times {{4.20}, {4.90}};
    return ADC_Times[instr.getMode ()].word_ +
        DestinationTimes[instr.getMode ()].word_;
}

double KD11_NA::Calculate::operator() (ADCB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const ADCB_Times {{3.85}, {4.55}};
    return ADCB_Times[instr.getMode ()].byte_ +
        DestinationTimes[instr.getMode ()].byte_;
}

double KD11_NA::Calculate::operator() (SBC const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const SBC_Times {{4.20}, {4.90}};
    return SBC_Times[instr.getMode ()].word_ +
        DestinationTimes[instr.getMode ()].word_;
}

double KD11_NA::Calculate::operator() (SBCB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const SBCB_Times {{3.85}, {4.55}};
    return SBCB_Times[instr.getMode ()].byte_ +
        DestinationTimes[instr.getMode ()].byte_;
}

double KD11_NA::Calculate::operator() (SXT const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const SXT_Times {{5.95}, {6.65}};
    return SXT_Times[instr.getMode ()].word_ +
        DestinationTimes[instr.getMode ()].word_;
}

// The LSI-11 PDP-11/03 Processor Handbook (appendix B) states that for the
// MTPS instruction byte DST time is used instead of SRC time. This suggests
// that for the MFPS instruction the *byte* DST time has to be used instead
// of SRC time.
double KD11_NA::Calculate::operator() (MFPS const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const MFPS_Times {{4.90}, {6.65}};
    return MFPS_Times[instr.getMode ()].byte_ +
        DestinationTimes[instr.getMode ()].byte_;
}

// The LSI-11 PDP-11/03 Processor Handbook (appendix B) states that for the
// MTPS instruction byte DST time is used instead of SRC time.
double KD11_NA::Calculate::operator() (MTPS const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const MTPS_Times {{7.00}, {7.00}};
    return MTPS_Times[instr.getMode ()].byte_ +
        DestinationTimes[instr.getMode ()].byte_;
}

double KD11_NA::Calculate::operator() (JMP const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const JMP_Times
    {
        {{0.0}},    // Mode 0 Not used
        {{0.70}},   // Mode 1
        {{1.40}},   // Mode 2
        {{2.10}},   // Mode 3
        {{1.40}},   // Mode 4
        {{2.80}},   // Mode 5
        {{2.80}},   // Mode 6
        {{4.90}}    // Mode 7
    };

    return 3.50 + JMP_Times[instr.getMode ()].word_;
}

double KD11_NA::Calculate::operator() (MARK const& instr) const
{
    return 11.55;
}

// The MFPD, MFPI, MTPD and MTPI instructions are unsupported on the KD11-NA.
// Added to satisfy the compiler.
double KD11_NA::Calculate::operator() (MFPD const& instr) const
{
    return 0.0;
}

double KD11_NA::Calculate::operator() (MFPI const& instr) const
{
    return 0.0;
}

double KD11_NA::Calculate::operator() (MTPD const& instr) const
{
    return 0.0;
}

double KD11_NA::Calculate::operator() (MTPI const& instr) const
{
    return 0.0;
}

// Double operand instructions
double KD11_NA::Calculate::operator() (MOV const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const MOV_Times {{3.50}, {2.45}};
    return MOV_Times[instr.getDestinationMode ()].word_ +
        SourceTimes[instr.getSourceMode ()].word_ +
        DestinationTimes[instr.getDestinationMode ()].word_;
}

double KD11_NA::Calculate::operator() (MOVB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const MOVB_Times {{3.85}, {3.85}};
    return MOVB_Times[instr.getDestinationMode ()].byte_ +
        SourceTimes[instr.getSourceMode ()].byte_ +
        DestinationTimes[instr.getDestinationMode ()].byte_;
}

double KD11_NA::Calculate::operator() (CMP const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const CMP_Times {{3.50}, {3.15}};
    return CMP_Times[instr.getDestinationMode ()].word_ +
        SourceTimes[instr.getSourceMode ()].word_ +
        DestinationTimes[instr.getDestinationMode ()].word_;
}

double KD11_NA::Calculate::operator() (CMPB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const CMPB_Times {{3.15}, {2.80}};
    return CMPB_Times[instr.getDestinationMode ()].byte_ +
        SourceTimes[instr.getSourceMode ()].byte_ +
        DestinationTimes[instr.getDestinationMode ()].byte_;
}

double KD11_NA::Calculate::operator() (ADD const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const ADD_Times {{3.50}, {4.20}};
    return ADD_Times[instr.getDestinationMode ()].word_ +
        SourceTimes[instr.getSourceMode ()].word_ +
        DestinationTimes[instr.getDestinationMode ()].word_;
}

double KD11_NA::Calculate::operator() (SUB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const SUB_Times {{3.50}, {4.20}};
    return SUB_Times[instr.getDestinationMode ()].word_ +
        SourceTimes[instr.getSourceMode ()].word_ +
        DestinationTimes[instr.getDestinationMode ()].word_;
}

double KD11_NA::Calculate::operator() (BIT const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const BIT_Times {{3.50}, {3.15}};
    return BIT_Times[instr.getDestinationMode ()].word_ +
        SourceTimes[instr.getSourceMode ()].word_ +
        DestinationTimes[instr.getDestinationMode ()].word_;
}

double KD11_NA::Calculate::operator() (BITB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const BITB_Times {{3.15}, {2.80}};
    return BITB_Times[instr.getDestinationMode ()].byte_ +
        SourceTimes[instr.getSourceMode ()].byte_ +
        DestinationTimes[instr.getDestinationMode ()].byte_;
}

double KD11_NA::Calculate::operator() (BIC const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const BIC_Times {{3.50}, {4.20}};
    return BIC_Times[instr.getDestinationMode ()].word_ +
        SourceTimes[instr.getSourceMode ()].word_ +
        DestinationTimes[instr.getDestinationMode ()].word_;
}

double KD11_NA::Calculate::operator() (BICB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const BICB_Times {{3.85}, {3.85}};
    return BICB_Times[instr.getDestinationMode ()].byte_ +
        SourceTimes[instr.getSourceMode ()].byte_ +
        DestinationTimes[instr.getDestinationMode ()].byte_;
}

double KD11_NA::Calculate::operator() (BIS const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const BIS_Times {{3.50}, {4.20}};
    return BIS_Times[instr.getDestinationMode ()].word_ +
        SourceTimes[instr.getSourceMode ()].word_ +
        DestinationTimes[instr.getDestinationMode ()].word_;
}

double KD11_NA::Calculate::operator() (BISB const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const BISB_Times {{3.85}, {3.85}};
    return BISB_Times[instr.getDestinationMode ()].byte_ +
        SourceTimes[instr.getSourceMode ()].byte_ +
        DestinationTimes[instr.getDestinationMode ()].byte_;
}

// Eis instructions, including JSR and XOR
double KD11_NA::Calculate::operator() (JSR const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const JSR_Times
    {
        {{0.0}},    // Mode 0 Not used
        {{0.70}},   // Mode 1
        {{1.40}},   // Mode 2
        {{2.10}},   // Mode 3
        {{1.40}},   // Mode 4
        {{2.80}},   // Mode 5
        {{2.80}},   // Mode 6
        {{4.90}}    // Mode 7
    };

    return (instr.getRegisterNr () == 7 ? 5.25 : 6.40) +
        JSR_Times[instr.getOperandMode ()].word_;
}

// If both numbers are less than 256 in absolute value, the instruction
// time varies from 24.0 to 37.0 microseconds. A 16-bit multiplication is
// worst case 64.0 microseconds. As exact instruction times are not specified
// we have to make an assumption. And as the SRC times are significantly less
// than the instruction time and the exact instruction time is unknown,
// adding the SRC time has little use. We therefor fix the instruction time
// at 40 microseconds.
//
double KD11_NA::Calculate::operator() (MUL const& instr) const
{
    return 40.0;
}

// The LSI-11/PDP-11.03 handbook for the DIV instruction just a worst case
// time of 78 microseconds. We'll assume an average instruction time of
// 39 microseconds plus 3 microseconds SRC time.
double KD11_NA::Calculate::operator() (DIV const& instr) const
{
    return 42.0;
}

double KD11_NA::Calculate::operator() (ASH const& instr) const
{
    if (instr.getShiftCount () > 0)
        // Positive is a left shift
        return 10.8 + 2.45 * instr.getShiftCount ();
    else
        // Negative is a right shift
        // Zero is no shift
        return 10.1 + 1.75 * -instr.getShiftCount ();
}

double KD11_NA::Calculate::operator() (ASHC const& instr) const
{
    if (instr.getShiftCount () > 0)
        // Positive is a left shift
        return 10.1 + 3.15 * instr.getShiftCount ();
    else
        // Negative is a right shift
        // Zero is no shift
        return 10.1 + 2.80 * -instr.getShiftCount ();
}

// The XOR instruction has the EIS instruction format but the timing of
// a double operand instruction for the destination operand.
double KD11_NA::Calculate::operator() (XOR const& instr) const
{
    static AddressingModeTimes<KD11_NA_AddressingModeTime> const XOR_Times {{3.50}, {4.20}};
    return XOR_Times[instr.getOperandMode ()].word_ +
        DestinationTimes[instr.getOperandMode ()].word_;
}

// The timing of the SOB instruction depends on wheter or not the branch is
// taken. The SOB instruction usually will branch more often than not and the
// times vary just 0.7 microseconds. Therfore the branch time is used in the
// calculation.
double KD11_NA::Calculate::operator() (SOB const& instr) const
{
    return 4.90;
}

// FIS format instructions
double KD11_NA::Calculate::operator() (RTS const& instr) const
{
    return 5.25;
}

// The instruction time is the basic time plus shift time for binary points
// plus the shift time for normalization. The shift times depend on the
// exponent difference and to be able to calculate this the operands have
// to be retrieved this is currently beyond the scope of the instruction
// time calculation. Instead we use the basic time plus an average for the
// exponent difference (12 shifts).
double KD11_NA::Calculate::operator() (FADD const& instr) const
{
    return 75.0;
}

// See comment on FADD
double KD11_NA::Calculate::operator() (FSUB const& instr) const
{
    return 75.0;
}

// Typical execution time for FDIV is 151 microseconds. We presume the same
// will hold for FMUL.
double KD11_NA::Calculate::operator() (FMUL const& instr) const
{
    return 151.0;
}

// Typical execution time for FDIV is 151 microseconds.
double KD11_NA::Calculate::operator() (FDIV const& instr) const
{
    return 151.0;
}

// Branch instructions
// The execution time of all branch instructions is 3.50 microseconds
// whether the condition is met or not.
double KD11_NA::Calculate::operator() (BR const& instr) const
{
    return 3.50;
}

double KD11_NA::Calculate::operator() (BNE const& instr) const
{
    return 3.50;
}

double KD11_NA::Calculate::operator() (BEQ const& instr) const
{
    return 3.50;
}

double KD11_NA::Calculate::operator() (BPL const& instr) const
{
    return 3.50;
}

double KD11_NA::Calculate::operator() (BMI const& instr) const
{
    return 3.50;
}

double KD11_NA::Calculate::operator() (BVC const& instr) const
{
    return 3.50;
}

double KD11_NA::Calculate::operator() (BVS const& instr) const
{
    return 3.50;
}

double KD11_NA::Calculate::operator() (BCC const& instr) const
{
    return 3.50;
}

double KD11_NA::Calculate::operator() (BCS const& instr) const
{
    return 3.50;
}

double KD11_NA::Calculate::operator() (BGE const& instr) const
{
    return 3.50;
}

double KD11_NA::Calculate::operator() (BLT const& instr) const
{
    return 3.50;
}

double KD11_NA::Calculate::operator() (BGT const& instr) const
{
    return 3.50;
}

double KD11_NA::Calculate::operator() (BLE const& instr) const
{
    return 3.50;
}

double KD11_NA::Calculate::operator() (BHI const& instr) const
{
    return 3.50;
}

double KD11_NA::Calculate::operator() (BLOS const& instr) const
{
    return 3.50;
}

// If the new PSW has bit 4 or bit 7 set 0.35 microseconds is added to the
// instruction time. As we have no access to the vector and 0.35 is a minimal
// addition compared with the basic instruction time (16.8 microseconds) we
// just count the basic instruction time.
double KD11_NA::Calculate::operator() (EMT const& instr) const
{
    return 16.80;
}

// See comment on EMT above.
double KD11_NA::Calculate::operator() (TRAP const& instr) const
{
    return 16.80;
}

// No-operand instructions
double KD11_NA::Calculate::operator() (HALT const& instr) const
{
    return 5.60;
}

double KD11_NA::Calculate::operator() (WAIT const& instr) const
{
    return 6.30;
}

// See comment on EMT instruction above.
double KD11_NA::Calculate::operator() (RTI const& instr) const
{
    return 8.75;
}

// The LSI-11/PDP-11/03 Handbook contains a typo in the timing for the BPT
// instruction. The timing for the (non-existing) RPT instruction probably has
// to be read as the timing for the BPT instruction.
double KD11_NA::Calculate::operator() (BPT const& instr) const
{
    return 18.55;
}

// See comment on EMT instruction above.
double KD11_NA::Calculate::operator() (IOT const& instr) const
{
    return 18.55;
}

// A RESET takes 5.95 + 10.0 microsec. for INIT + 90.0 microsec.
double KD11_NA::Calculate::operator() (RESET const& instr) const
{
    return 105.95;
}

// If the new PSW has bit 4 or bit 7 set 0.35 microseconds is added to the
// instruction time. If the new PSW has the T-bit set another
//  2.10 microseconds is added. As we have no access to the vector we just
// count the basic instruction time.
double KD11_NA::Calculate::operator() (RTT const& instr) const
{
    return 8.75;
}

// All clear condition code instructions are executed in 3.50 microseconds.
double KD11_NA::Calculate::operator() (CCC const& instr) const
{
    return 3.50;
}

// All set condition code instructions are executed in 3.50 microseconds.
double KD11_NA::Calculate::operator() (SCC const& instr) const
{
    return 3.50;
}

// The MFPY instruction is not supported on the KD11-NA.
double KD11_NA::Calculate::operator() (MFPT const& instr) const
{
    return 0.0;
}

// Unused instructions naturally have no execution time, but the operator()
// function has to be defined to be able to compile the class.
double KD11_NA::Calculate::operator() (Unused const& instr) const
{
    return 0.0;
}