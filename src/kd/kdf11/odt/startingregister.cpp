#include "kdf11_odt.h"

KDF11_ODT::State KDF11_ODT::StateMachine::transition (StartingRegister_2 &&, DigitEntered digitEntered)
{
    // Append the given digit to the series of digits entered. This is to keep
    // track of the last digits entered.
    // //
    // If more than one character (digit or S) follows the R or $, OOT will use
    // the last character as the register designator. An exception: if the last
    // three digits equal 077 or 477, OOT will open the PS rather than R7.
    // (EK-KDJ1A-UG-001)
    context_->registerSeries_.push_back (digitEntered.digit);
    return EnteringRegister_6 {};
}

KDF11_ODT::State KDF11_ODT::StateMachine::transition (StartingRegister_2 &&, PswDesignatorEntered)
{
    context_->registerSeries_.push_back (PswDesignator);
    return EnteringRegister_6 {};
}