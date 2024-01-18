#include "kdf11_a_odt.h"

#include <cassert>

using std::move;
using std::monostate;

KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringRegisterValue_8 &&, DigitEntered digitEntered)
{
    context_->digitSeries_.push_back (digitEntered.digit);
    return EnteringRegisterValue_8 {};
}

// See the comment at the equivalent EnteringAddressValue_7 state
KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringRegisterValue_8 &&currentState, RuboutEntered)
{
    // We expect there always is at least one character in the digitSeries_
    // buffer available. This either is a digit entered by the user or a '0'
    // in case the user has typed a RUBOUT in the AddressOpened_3 state.
    assert (!context_->digitSeries_.empty ());

    // Echo a backslash and remove the last entered character. If the 
    // digitSeries_ is now empty replace it with a '0'.
    context_->console_->write ('\\');
    context_->digitSeries_.pop_back ();
    if (context_->digitSeries_.empty ())
        context_->digitSeries_.push_back ('0');

    return move (currentState);
}

KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringRegisterValue_8 &&, CloseLocationCmdEntered)
{   
    context_->setRegisterValue ();
    context_->writeString ("\n");
    return AtPrompt_1 {};
}

// When the user enters an Open location command (/) the given value has to be
// used as an address to open.
KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringRegisterValue_8&&, OpenLocationCmdEntered)
{
    return move (context_->openAddress ());
}

KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringRegisterValue_8 &&, OpenNextLocationCmdEntered)
{
    context_->setRegisterValue ();
    return move (context_->openNextRegister (RegisterOpened_4 {},
        [this] () {return (context_->location_.registerNr () + 1) % 8;}));
}


// Note that this [back arrow] command cannot be used if a GPR or the PS is
// the open location and if attempted, the command will modify the GPR
// or PS if data has been typed, and close the GPR or PS; then a CR,
// LF, @ will be issued. (LSI11 PDP11/03 Processor Handbook)
KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringRegisterValue_8 &&, BackArrowCmdEntered)
{   
    context_->setRegisterValue ();
    // The cursor is positioned just after the _, so write a new line
    // to position the cursor at the begin of the new line.
    context_->writeString ("\n");
    return AtPrompt_1 {};
}
