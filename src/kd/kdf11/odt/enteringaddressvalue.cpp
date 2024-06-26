#include "kdf11_odt.h"

#include <cassert>

// This file contains the entry actions and state transitions for
// the state EnteringAddressValue_7.
KDF11_ODT::State KDF11_ODT::StateMachine::transition (EnteringAddressValue_7 &&, DigitEntered digitEntered)
{ 
    context_->digitSeries_.push_back (digitEntered.digit);
    return EnteringAddressValue_7 {};
}

KDF11_ODT::State KDF11_ODT::StateMachine::transition (EnteringAddressValue_7 &&, CloseLocationCmdEntered)
{
    context_->setAddressValue ();
    context_->writeString ("\n");
    return AtPrompt_1 {};
}

KDF11_ODT::State KDF11_ODT::StateMachine::transition (EnteringAddressValue_7 &&, OpenNextLocationCmdEntered)
{
    context_->setAddressValue ();
    return context_->openNextAddress ();
}
