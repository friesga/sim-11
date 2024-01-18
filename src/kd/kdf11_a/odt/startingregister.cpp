#include "kdf11_a_odt.h"

KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (StartingRegister_2 &&, DigitEntered digitEntered)
{
    // Append the given digit to the series of digits entered. This is to keep
    // track of the last digits entered.
    // //
    // If more than one character (digit or S) follows the R or $, OOT will use
    // the last character as the register designator. An exception: if the last
    // three digits equal 077 or 477, OOT will open the PS rather than R7.
    // (EK-KDJ1A-UG-001)
    //
    // Test runs executed on a LSI-11/2 show that this is not correct. When
    // more than one digit is entered, the digits are interpreted as an
    // address to be opened.
    //
    context_->registerSeries_.push_back (digitEntered.digit);
    return EnteringRegister_6 {};
}

KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (StartingRegister_2 &&, PswDesignatorEntered)
{
    context_->registerSeries_.push_back (PswDesignator);
    return EnteringRegister_6 {};
}