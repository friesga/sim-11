#include "kd11odt.h"

using namespace KD11_ODT;

// This file contains all transition for the state EnteringRegister_6.
// In this state two cases have to be treated: the entry and display of
// a register and the entry and display of the PSW. I would have preferred
// to implement these cases as different (sub)states, but the variantfsm
// statemachine offers no functionality for substates and I prefer the
// implemented statemachine to resemble the statemachine described in the
// KDJ11 User Guide as closely as possible.

State KD11ODT::transition (EnteringRegister_6 &&, DigitEntered digitEntered)
{
    // Append the entered digit to series of digits entered. See comment
    // on the transition in the StartingRegister_5 state.
    if (registerSeries_.empty ())
    {
        registerSeries_.push_back (digitEntered.digit);
        return EnteringRegister_6 {};
    }
    else
    {
        digitSeries_ = registerSeries_;
        digitSeries_.push_back (digitEntered.digit);
        registerSeries_.clear ();
        return EnteringAddress_5 {};
    }
}

// The RUBOUT command cannot be used while entering a register number.
// R2\4/012345 will not open register R4; however the RUBOUT command will
// cause ODT to revert to memory mode and open location 4.
// (LSI11 PDP11/03 Processor Handbook)
//
// This implies that on a RUBOUT a transition to the AddressOpened_3 state
// has to performed. A RuboutEntered event can only be triggered when one
// digit has been entered as a second digit will already have caused a
// transition to the EnteringAddress_5 state. So on a RuboutEntered event
// in this (i.e. the EnteringRegister_6) state, both registerSeries_ and
// digitSeries_ have to be cleared.
State KD11ODT::transition (EnteringRegister_6 &&, RuboutEntered)
{
    console_->write ('\\');
    registerSeries_.clear ();
    digitSeries_.clear ();
    return EnteringAddress_5 {};
}

State KD11ODT::transition (EnteringRegister_6 &&, PswDesignatorEntered)
{
    registerSeries_.push_back (PswDesignator);
    return EnteringRegister_6 {};
}

// According to EK-KDJ1A-UG-001 the last three digits indicates whether
// a register or the PSW has to be opened. At least for a LSI-11/2 this
// is not correct. If more than one register digit is entered the digits
// are interpreted as an address to be opened.
State KD11ODT::transition (EnteringRegister_6 &&, OpenLocationCmdEntered)
{
    // The last character in the registerSeries indicates whether a register
    // and in that case which register or the PSW has to be openend.
    if (registerSeries_.back() == PswDesignator)
    {
        // Open PSW. Only set it as a new open location if it wasn't already
        // the open location. This way the previously opened memory or
        // register location can be accessed, even if the PSW is openend
        // multiple times. This is used in the processing of the at sign
        // command for an opened PSW (see the gtransition from
        // EnteringRegisterValue_8 for the AtSignCmdEntered event).
        writeString (octalNumberToString (cpu_.pswValue ()) + ' ');
        if (!location_.isA<PSWLocation> ())
            location_ = PSWLocation {};
    }
    else
    {
        // Open register
        location_ = RegisterLocation {static_cast<u8> (registerSeries_.back() - '0')};
        writeString (octalNumberToString (cpu_.registerValue (location_.registerNr ())) + ' ');
    }

    return RegisterOpened_4 {};
}

// Test runs on a real LSI-11/2 showed that an address location can be closed
// before it is opened. Presumably the same holds for a register location.
State KD11ODT::transition (EnteringRegister_6 &&, CloseLocationCmdEntered)
{
    writeString ("\n");
    return AtPrompt_1 {};
}