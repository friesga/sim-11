#include "kdf11_a_odt.h"

KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (AddressOpened_3 &&, DigitEntered digitEntered)
{
    context_->digitSeries_ = digitEntered.digit;
    return EnteringAddressValue_7 {};
}

// A RUBOUT is entered while there is no digit for the new value entered.
// Echo a backslash character, put a '0' in the digitSeries_ buffer and
// transition to the EnteringAddressValue_7 state. This way the default
// new value will be zero when more characters are erased than are available
// in the buffer. See the comment at the RuboutEntered transition in the
// EnteringAddressValue_7 state.
KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (AddressOpened_3 &&, RuboutEntered)
{
    context_->console_->write ('\\');
    context_->digitSeries_ = '0';
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
    return context_->openNextAddress ([this] () {return context_->location_.inputAddress () + 2;});
}


// Micronote 050 (Micro ODT Differences - LSI-11 vs. LSI-11/23) states that
// when an address location is open, another location can be opened without
// explicitly closing the first location. Tests conducted at a real LSi-11/2
// show that the same holds for register locations, i.e. when an address
// location is open a register location can be opened and when a register
// location is open an address or register location can be opened.
KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (AddressOpened_3&&, RegisterCmdEntered)
{
    return StartingRegister_2 {};
}