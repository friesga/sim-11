#include "kdf11_a_odt.h"

// This file contains all transition for the state EnteringRegister_6.
// In this state two cases have to be treated: the entry and display of
// a register and the entry and display of the PSW. I would have preferred
// to implement these cases as different (sub)states, but the variantfsm
// statemachine offers no functionality for substates and I prefer the
// implemented statemachine to resemble the statemachine described in the
// KDJ11 User Guide as closely as possible.

KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringRegister_6 &&, DigitEntered digitEntered)
{
    // Append the entered digit to series of digits entered. See comment
    // on the transition in the StartingRegister_5 state.
    if (context_->registerSeries_.empty ())
    {
        context_->registerSeries_.push_back (digitEntered.digit);
        return EnteringRegister_6 {};
    }
    else
    {
        context_->digitSeries_ = context_->registerSeries_;
        context_->digitSeries_.push_back (digitEntered.digit);
        context_->registerSeries_.clear ();
        return EnteringAddress_5 {};
    }
}

KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringRegister_6 &&, PswDesignatorEntered)
{
    context_->registerSeries_.push_back (PswDesignator);
    return EnteringRegister_6 {};
}

// According to EK-KDJ1A-UG-001 the last three digits indicates whether
// a register or the PSW has to be opened. At least for a LSI-11/2 this
// is not correct. If more than one register digit is entered the digits
// are interpreted as an address to be opened.
KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringRegister_6 &&, OpenLocationCmdEntered)
{
    // The last character in the registerSeries indicates whether a register
    // and in that case which register or the PSW has to be openend.
    if (context_->registerSeries_.back() == PswDesignator)
    {
        // Open PSW. Only set it as a new open location if it wasn't already
        // the open location. This way the previously opened memory or
        // register location can be accessed, even if the PSW is openend
        // multiple times.
        context_->writeString (context_->octalNumberToString (context_->cpuData_->psw ()) + ' ');
        if (!context_->location_.isA<PSWLocation> ())
            context_->location_ = PSWLocation {};
    }
    else
    {
        // Open register
        context_->location_ = RegisterLocation {static_cast<u8> (context_->registerSeries_.back() - '0')};
        context_->writeString (context_->octalNumberToString (context_->cpuData_->registers () [context_->location_.registerNr ()]) + ' ');
    }

    return RegisterOpened_4 {};
}

// Test runs on a real LSI-11/2 showed that an address location can be closed
// before it is opened. Presumably the same holds for a register location.
KDF11_A_ODT::State KDF11_A_ODT::StateMachine::transition (EnteringRegister_6 &&, CloseLocationCmdEntered)
{
    context_->writeString ("\n");
    return AtPrompt_1 {};
}