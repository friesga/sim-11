#include "kd11odt.h"

using namespace KD11_ODT;

using std::move;

State KD11ODT::transition (RegisterOpened_4 &&, CloseLocationCmdEntered)
{
    console_->write ('\n');
    return AtPrompt_1 {};
}

State KD11ODT::transition (RegisterOpened_4 &&, DigitEntered digitEntered)
{
    digitSeries_ = digitEntered.digit;
    return EnteringRegisterValue_8 {};
}

State KD11ODT::transition (RegisterOpened_4 &&, RuboutEntered)
{
    console_->write ('\\');
    digitSeries_ = '0';
    return EnteringRegisterValue_8 {};
}

// This [<LF>] command is used to close an open location or GPR (general
// purpose register). If entered after a location has been opened, it will
// close the open location or GPR and open location+2 or GPR+1. If the
// contents of the open location or GPR are to be modified, the new
// contents should precede the <LF> operator.
// 
// If <LF> is used to advance beyond R7, the register name printed is
// meaningless, but the contents printed are those of R0.
// (Microcomputers and Memories Chapter 7)
//
// This last behaviour is not implemented as 1) it clearly is a (documented)
// bug and 2) the real behaviour is not completely described.
// 
// This function will transition either to the current state or to atPrompt_1
// in case the PSW is openend.
State KD11ODT::transition (RegisterOpened_4 &&currentState, OpenNextLocationCmdEntered)
{
    return move (openNextRegister (move (currentState),
        [this] () {return (location_.registerNr () + 1) % 8;}));
}

// The "up arrow" command is also used to close an open location or GPR. If
// entered after a location or GPR has been opened, it will close the open
// location or GPR and open location-2, or GPR-1.lf the contents of the
//open location or GPR are to be modified, the new contents should
// precede the "up arrow" operator.
// 
// If "up arrow" is used to decrement below R0, the register name printed is
// meaningless but the content is that of R7.
// (Microcomputers and Memories Chapter 7)
// 
// This last behaviour is not implemented as 1) it clearly is a (documented)
// bug and 2) the real behaviour is not completely described.
// 
// Process the OpenPreviousLocationCmdEntered event. This function will transition
// either to the current state or to atPrompt_1 in case the PSW is openend.
State KD11ODT::transition (RegisterOpened_4 &&currentState, OpenPreviousLocationCmdEntered)
{
    console_->write ('\n');
    return move (openNextRegister (move (currentState),
        [this] () {return static_cast<u8> (location_.registerNr () - 1) % 8;}));
}

State KD11ODT::transition (RegisterOpened_4 &&currentState, AtSignCmdEntered)
{
    console_->write ('\n');
    return openNextAddress ([this] () 
    {
        return location_.isA<RegisterLocation> () ?
            cpu_.register_[location_.registerNr ()] : cpu_.psw;
    });
}

// Note that this [back arrow] command cannot be used if a GPR or the PS is
// the open location and if attempted, the command will modify the GPR
// or PS if data has been typed, and close the GPR or PS; then a CR,
// LF, @ will be issued. (LSI11 PDP11/03 Processor Handbook)
State KD11ODT::transition (RegisterOpened_4 &&, BackArrowCmdEntered)
{
    // The cursor is positioned just after the _, so write a new line
    // to position the cursor at the begin of the new line.
    writeString ("\n");
    return AtPrompt_1 {};
}