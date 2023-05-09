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
    registerSeries_.push_back (digitEntered.digit);
    return EnteringRegister_6 {};
}

// The RUBOUT command cannot be used while entering a register number.
// R2\4/012345 will not open register R4; however the RUBOUT command will
// cause ODT to revert to memory mode and open location 4.
// (LSI11 PDP11/03 Processor Handbook)
//
// This implies that on a RUBOUT a transition to the AddressOpened_3 state.
State KD11ODT::transition (EnteringRegister_6 &&, RuboutEntered)
{
    character.write ('\\');
    return EnteringAddress_5 {};
}

State KD11ODT::transition (EnteringRegister_6 &&, PswDesignatorEntered)
{
    registerSeries_.push_back (PswDesignator);
    return EnteringRegister_6 {};
}

State KD11ODT::transition (EnteringRegister_6 &&, OpenLocationCmdEntered)
{
    // The last character in the registerSeries indicates whether a register
    // and in that case which register or the PSW has to be openend. An
    // exception has to be made when the last three digits entered are 077
    // or 477; in that case the PSW has to be opened. (EK-KDJ1A-UG-001)
    if (registerSeries_.back() == PswDesignator || 
        registerSeriesEndsWith ("077") ||
        registerSeriesEndsWith ("477"))
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