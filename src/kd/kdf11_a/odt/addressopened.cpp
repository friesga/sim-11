#include "kdf11_a_odt.h"

KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (AddressOpened_3 &&, DigitEntered digitEntered)
{
    context_->digitSeries_ = digitEntered.digit;
    return EnteringAddressValue_7 {};
}

// This file contains the state transitions for the state AddressOpened_3.
KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (AddressOpened_3 &&, CloseLocationCmdEntered)
{
    context_->writeString ("\n");
    return AtPrompt_1 {};
}

// This [open next location] command is also used to close an open location
// or GPR (general purpose register). If entered after a location has been
// opened, it will close the open location or GPR and open location + 2 or
// GPR + l. If the contents of the open location or GPR are
// to be modified, the new contents should precede the LF operator.
// (Microcomputer and Memories Chapter 7)
KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (AddressOpened_3 &&, OpenNextLocationCmdEntered)
{
    return context_->openNextAddress ();
}
