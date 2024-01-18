#include "kdf11_a_odt.h"

#include <cassert>

using std::move;
using std::monostate;

KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringRegisterValue_8 &&, DigitEntered digitEntered)
{
    context_->digitSeries_.push_back (digitEntered.digit);
    return EnteringRegisterValue_8 {};
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
