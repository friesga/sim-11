#include "kdf11_a_odt.h"

#include <cassert>

// This file contains the entry actions and state transitions for
// the state EnteringAddressValue_7.
KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringAddressValue_7 &&, DigitEntered digitEntered)
{ 
    context_->digitSeries_.push_back (digitEntered.digit);
    return EnteringAddressValue_7 {};
}

// If more than six RUBOUTS are consecutively typed, and then a valid location
// closing command is typed, the open location will be modified with all
// zeroes. (LSI11 PDP11/03 Processor Handbook)
//
// I presume the last remark means that when more RUBOUTS are entered then
// characters are available in the buffer (digitSeries_), the open location
// will be set to zero. In this case we replace the last character by a
// zero. This is an ugly solution as the value typed in by the user now has
// a leading zero, but making a distinction between an empty string indicating
// there are no characters to be converted to a value and an empty string
// which should be converted to zero will create an even uglier solution.
KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringAddressValue_7 &&currentState, RuboutEntered)
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

KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringAddressValue_7 &&, CloseLocationCmdEntered)
{
    context_->setAddressValue ();
    context_->writeString ("\n");
    return AtPrompt_1 {};
}

KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringAddressValue_7 &&, OpenNextLocationCmdEntered)
{
    context_->setAddressValue ();
    return context_->openNextAddress ([this] () {return context_->location_.inputAddress () + 2;});
}

KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringAddressValue_7 &&, OpenPreviousLocationCmdEntered)
{
    context_->writeString ("\n");
    context_->setAddressValue ();
    return context_->openNextAddress ([this] () {return context_->location_.inputAddress () - 2;});
}

// Also the open location can be optionally modified similar to other commands
// and if done, the new contents will be used as the pointer.
// (LSI11 PDP11/03 Processor Handbook)
KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringAddressValue_7 &&, AtSignCmdEntered)
{
    context_->writeString ("\n");
    context_->setAddressValue ();
    return context_->openNextAddress ([this] () {return context_->newValue_;});
}

KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringAddressValue_7 &&, BackArrowCmdEntered)
{
    context_->writeString ("\n");
    context_->setAddressValue ();
    return context_->openNextAddress ([this] () 
        {return context_->location_.wordAddress () + context_->bus_->read (context_->location_.wordAddress ()) + 2;});
}

// When an address location is open, another location can be opened without
// explicitly closing the first location; e.g., 1000/123456 2000/054321.
// (Micronote 050 Micro ODT Differences - LSI-11 vs. LSI-11/23)
KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringAddressValue_7 &&, OpenLocationCmdEntered)
{
    return move (context_->openAddress ());
}