#include "kdf11_a_odt.h"

using std::move;

KDF11_ODT::State KDF11_ODT::StateMachine::transition (RegisterOpened_4 &&, CloseLocationCmdEntered)
{
    context_->writeString ("\n");
    return AtPrompt_1 {};
}

KDF11_ODT::State KDF11_ODT::StateMachine::transition (RegisterOpened_4 &&, DigitEntered digitEntered)
{
    context_->digitSeries_ = digitEntered.digit;
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
KDF11_ODT::State KDF11_ODT::StateMachine::transition (RegisterOpened_4 &&currentState, OpenNextLocationCmdEntered)
{
    return move (context_->openNextRegister (move (currentState),
        [this] () {return (context_->location_.registerNr () + 1) % 8;}));
}
