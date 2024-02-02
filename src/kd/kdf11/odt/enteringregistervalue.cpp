#include "kdf11_a_odt.h"

#include <cassert>

using std::move;
using std::monostate;

KDF11_ODT::State KDF11_ODT::StateMachine::transition (EnteringRegisterValue_8 &&, DigitEntered digitEntered)
{
    context_->digitSeries_.push_back (digitEntered.digit);
    return EnteringRegisterValue_8 {};
}

KDF11_ODT::State KDF11_ODT::StateMachine::transition (EnteringRegisterValue_8 &&, CloseLocationCmdEntered)
{   
    context_->setRegisterValue ();
    context_->writeString ("\n");
    return AtPrompt_1 {};
}

KDF11_ODT::State KDF11_ODT::StateMachine::transition (EnteringRegisterValue_8 &&, OpenNextLocationCmdEntered)
{
    context_->setRegisterValue ();
    return move (context_->openNextRegister (RegisterOpened_4 {},
        [this] () {return (context_->location_.registerNr () + 1) % 8;}));
}
