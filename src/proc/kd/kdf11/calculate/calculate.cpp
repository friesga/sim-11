#include "calculate.h"

// Logic for calculating the execution time of the instructions.
// The AddressinModeTimes arrays are defined static to avoid creating
// the arrays at every instruction execution.
//
// Single operand instructions
//
// The following instructions use the MOV basic timing plus the MOV
// destination timing.
double KDF11_Calculate::operator() (CLR const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group1Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (CLRB const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group1Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (COM const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group1Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (COMB const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group1Times[instr.getMode ()].MMEnabled_;
}

// The following instructions use the MOV basic timing plus the ADD
// destination timing.
double KDF11_Calculate::operator() (INC const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_  +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (INCB const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (DEC const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (DECB const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (NEG const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (NEGB const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;
}

// The following instructions use the MOV basic timing plus the CMP(B)
// destination timing.
double KDF11_Calculate::operator() (TST const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group2Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (TSTB const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group2Times[instr.getMode ()].MMEnabled_;
}

// The following instructions use the MOV basic timing plus the ADD
// destination timing.
double KDF11_Calculate::operator() (ASR const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (ASRB const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (ASL const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (ASLB const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (ROR const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (RORB const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (ROL const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (ROLB const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;;
}

double KDF11_Calculate::operator() (SWAB const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (ADC const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (ADCB const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (SBC const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;;
}

double KDF11_Calculate::operator() (SBCB const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getMode ()].MMEnabled_;
}

// The following instructions use the MOV basic timing plus the MOV(B)
// destination timing.
double KDF11_Calculate::operator() (SXT const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group1Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (MFPS const& instr) const
{
    return basicB6Group1Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group1Times[instr.getMode ()].MMEnabled_;;
}

// MTPS uses the MTPS basic timing plus MUL source address timing
double KDF11_Calculate::operator() (MTPS const& instr) const
{
    return basicB6Group2Times[instr.getMode ()].MMEnabled_  +
        sourceB4Group2Times[instr.getMode ()].MMEnabled_;
}

// The JMP instruction has an addressing mode specific basic timing
double KDF11_Calculate::operator() (JMP const& instr) const
{
    return basicB7JMPTimes[instr.getMode ()].MMEnabled_;
}

// The MARK instruction has an non addressing mode specific timing.
double KDF11_Calculate::operator() (MARK const& instr) const
{
    return basicB6Group9Times[0].MMEnabled_;
}

// MFPD and MFPI use the MFPI basing timing plus the MUL source address
// timing.
double KDF11_Calculate::operator() (MFPD const& instr) const
{
    return basicB6Group3Times[instr.getMode ()].MMEnabled_ +
        sourceB4Group2Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (MFPI const& instr) const
{
    return basicB6Group3Times[instr.getMode ()].MMEnabled_ +
        sourceB4Group2Times[instr.getMode ()].MMEnabled_;
}

// MTPD and MTPI use the MTPI basic timing plus the MOV(B) source address timing
double KDF11_Calculate::operator() (MTPD const& instr) const
{
    return basicB6Group4Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group1Times[instr.getMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (MTPI const& instr) const
{
    return basicB6Group4Times[instr.getMode ()].MMEnabled_ +
        destinationB5Group1Times[instr.getMode ()].MMEnabled_;
}

// Double operand instructions
// 
// The MOV and MOVB instructions have the same timing
double KDF11_Calculate::operator() (MOV const& instr) const
{
    return basicB6Group1Times[instr.getDestinationMode ()].MMEnabled_ +
        sourceB4Group1Times[instr.getSourceMode ()].MMEnabled_ +
        destinationB5Group1Times[instr.getDestinationMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (MOVB const& instr) const
{
    return basicB6Group1Times[instr.getDestinationMode ()].MMEnabled_ +
        sourceB4Group1Times[instr.getSourceMode ()].MMEnabled_ +
        destinationB5Group1Times[instr.getDestinationMode ()].MMEnabled_;
}

// The CMP and CMPB instructions have the same timing too.
double KDF11_Calculate::operator() (CMP const& instr) const
{
    return basicB6Group1Times[instr.getDestinationMode ()].MMEnabled_ +
        sourceB4Group1Times[instr.getSourceMode ()].MMEnabled_ +
        destinationB5Group2Times[instr.getDestinationMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (CMPB const& instr) const
{
    return basicB6Group1Times[instr.getDestinationMode ()].MMEnabled_ +
        sourceB4Group1Times[instr.getSourceMode ()].MMEnabled_ +
        destinationB5Group2Times[instr.getDestinationMode ()].MMEnabled_;
}

// The ADD and SUB instruction have the same timing.
double KDF11_Calculate::operator() (ADD const& instr) const
{
    return basicB6Group1Times[instr.getDestinationMode ()].MMEnabled_ +
        sourceB4Group1Times[instr.getSourceMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getDestinationMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (SUB const& instr) const
{
    return basicB6Group1Times[instr.getDestinationMode ()].MMEnabled_ +
        sourceB4Group1Times[instr.getSourceMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getDestinationMode ()].MMEnabled_;
}

// The BIT and BITB instructions have the same timing
double KDF11_Calculate::operator() (BIT const& instr) const
{
    return basicB6Group1Times[instr.getDestinationMode ()].MMEnabled_ +
        sourceB4Group1Times[instr.getSourceMode ()].MMEnabled_ +
        destinationB5Group2Times[instr.getDestinationMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (BITB const& instr) const
{
    return basicB6Group1Times[instr.getDestinationMode ()].MMEnabled_ +
        sourceB4Group1Times[instr.getSourceMode ()].MMEnabled_ +
        destinationB5Group2Times[instr.getDestinationMode ()].MMEnabled_;;
}

// The BIC, BICB, BIS and BISB instructions have the same timing
double KDF11_Calculate::operator() (BIC const& instr) const
{
    return basicB6Group1Times[instr.getDestinationMode ()].MMEnabled_ +
        sourceB4Group1Times[instr.getSourceMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getDestinationMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (BICB const& instr) const
{
    return basicB6Group1Times[instr.getDestinationMode ()].MMEnabled_ +
        sourceB4Group1Times[instr.getSourceMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getDestinationMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (BIS const& instr) const
{
    return basicB6Group1Times[instr.getDestinationMode ()].MMEnabled_ +
        sourceB4Group1Times[instr.getSourceMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getDestinationMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (BISB const& instr) const
{
    return basicB6Group1Times[instr.getDestinationMode ()].MMEnabled_ +
        sourceB4Group1Times[instr.getSourceMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getDestinationMode ()].MMEnabled_;
}

// Eis instructions, including JSR and XOR
double KDF11_Calculate::operator() (JSR const& instr) const
{
    return basicB7JSRTimes[instr.getOperandMode ()].MMEnabled_;
}


double KDF11_Calculate::operator() (MUL const& instr) const
{
    return basicB6Group15Times[instr.getOperandMode ()].MMEnabled_ +
        sourceB4Group2Times[instr.getOperandMode ()].MMEnabled_;
}


double KDF11_Calculate::operator() (DIV const& instr) const
{
    return basicB6Group16Times[instr.getOperandMode ()].MMEnabled_ +
        sourceB4Group2Times[instr.getOperandMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (ASH const& instr) const
{
    return basicB6Group17Times[instr.getOperandMode ()].MMEnabled_ +
        sourceB4Group2Times[instr.getOperandMode ()].MMEnabled_;
}

double KDF11_Calculate::operator() (ASHC const& instr) const
{
    return basicB6Group18Times[instr.getOperandMode ()].MMEnabled_ +
        sourceB4Group2Times[instr.getOperandMode ()].MMEnabled_;
}

// Appendix B Table B-6 (Basic Times) lacks an entry for the
// XOR instruction (which would mean that an XOR in mode 0 would
// take no time). We presume its basic time equals the time for
// similar instructions.
double KDF11_Calculate::operator() (XOR const& instr) const
{
    return basicB6Group1Times[instr.getOperandMode ()].MMEnabled_ +
        destinationB5Group3Times[instr.getOperandMode ()].MMEnabled_;
}

// The timing of the SOB instruction depends on wheter or not the branch is
// taken. The SOB instruction usually will branch more often than not and the
// times vary just 0.3 microseconds. Therefore the branch time is used in the
// calculation.
double KDF11_Calculate::operator() (SOB const& instr) const
{
    return basicB6Group5Times[0].MMEnabled_;
}

// FIS format instructions
//
// The RTS instruction has no addressing mode.
double KDF11_Calculate::operator() (RTS const& instr) const
{
    return basicB6Group8Times[0].MMEnabled_;
}

// FIS instructions aren't supported on the KDF11
double KDF11_Calculate::operator() (FADD const& instr) const
{
    return 0.0;
}

double KDF11_Calculate::operator() (FSUB const& instr) const
{
    return 0.0;
}

double KDF11_Calculate::operator() (FMUL const& instr) const
{
    return 0.0;
}

double KDF11_Calculate::operator() (FDIV const& instr) const
{
    return 0.0;
}

// Branch instructions
//
// All branch instruction have the same timing.
double KDF11_Calculate::operator() (BR const& instr) const
{
    return basicB6Group6Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (BNE const& instr) const
{
    return basicB6Group6Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (BEQ const& instr) const
{
    return basicB6Group6Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (BPL const& instr) const
{
    return basicB6Group6Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (BMI const& instr) const
{
    return basicB6Group6Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (BVC const& instr) const
{
    return basicB6Group6Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (BVS const& instr) const
{
    return basicB6Group6Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (BCC const& instr) const
{
    return basicB6Group6Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (BCS const& instr) const
{
    return basicB6Group6Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (BGE const& instr) const
{
    return basicB6Group6Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (BLT const& instr) const
{
    return basicB6Group6Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (BGT const& instr) const
{
    return basicB6Group6Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (BLE const& instr) const
{
    return basicB6Group6Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (BHI const& instr) const
{
    return basicB6Group6Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (BLOS const& instr) const
{
    return basicB6Group6Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (EMT const& instr) const
{
    return basicB6Group13Times[0].MMEnabled_;
}

// See comment on EMT above.
double KDF11_Calculate::operator() (TRAP const& instr) const
{
    return basicB6Group13Times[0].MMEnabled_;
}

// No-operand instructions
//
// Instruction timing for the HALT instruction is missing
// from appendix B. As the timing of this instruction is not
// relevant for timing purposes we assume it takes no time.
double KDF11_Calculate::operator() (HALT const& instr) const
{
    return 0.0;
}

double KDF11_Calculate::operator() (WAIT const& instr) const
{
    return basicB6Group14Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (RTI const& instr) const
{
    return basicB6Group10Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (BPT const& instr) const
{
    return basicB6Group12Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (IOT const& instr) const
{
    return basicB6Group12Times[0].MMEnabled_;
}

// Instruction timing for the RESET instruction is missing
// from appendix B. Let's assume a standard time.
double KDF11_Calculate::operator() (RESET const& instr) const
{
    return basicB6Group1Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (RTT const& instr) const
{
    return basicB6Group11Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (MFPT const& instr) const
{
    return basicB6Group11Times[0].MMEnabled_;
}

// All condition code instructions have the same timing.
double KDF11_Calculate::operator() (CCC const& instr) const
{
    return basicB6Group7Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (SCC const& instr) const
{
    return basicB6Group7Times[0].MMEnabled_;
}

double KDF11_Calculate::operator() (Unused const& instr) const
{
    return 0.0;
}