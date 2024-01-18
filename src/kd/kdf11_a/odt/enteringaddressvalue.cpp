#include "kdf11_a_odt.h"

#include <cassert>

// This file contains the entry actions and state transitions for
// the state EnteringAddressValue_7.
KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringAddressValue_7 &&, DigitEntered digitEntered)
{ 
    context_->digitSeries_.push_back (digitEntered.digit);
    return EnteringAddressValue_7 {};
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

// When an address location is open, another location can be opened without
// explicitly closing the first location; e.g., 1000/123456 2000/054321.
// (Micronote 050 Micro ODT Differences - LSI-11 vs. LSI-11/23)
KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringAddressValue_7 &&, OpenLocationCmdEntered)
{
    return move (context_->openAddress ());
}