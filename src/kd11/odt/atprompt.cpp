#include "kd11odt.h"

using namespace KD11_F;

using std::move;
using std::to_string;

// This file contains the entry actions and state transitions for
// the state AtPrompt_1.
void KD11ODT::entry (AtPrompt_1)
{
    // Issue the prompt character
    writeCharacter ('@');
}

State KD11ODT::transition (AtPrompt_1 &&, DigitEntered digitEntered)
{
    // The given digit is the first digit of an address
    digitSeries_ = digitEntered.digit;
    return EnteringAddress_5 {};
}

State KD11ODT::transition (AtPrompt_1 &&, RegisterCmdEntered)
{
    registerSeries_.clear ();
    return StartingRegister_2 {};
}

// The / command can be used without a location specifier to verify the data
// just entered into a previously opened location. The / produces this result
// only if it is entered immediately after a prompt character. A / issued
// immediately after the processor enters OOT mode will cause ? <CR>, <LF> to
// be printed because a location has not yet been opened. (EK-KDJ1A-UG-001)
State KD11ODT::transition (AtPrompt_1 && currentState, OpenLocationCmdEntered)
{
    if (location_.isA<monostate> ())
    {
        // No location openend yet
        writeString ("?\n");
        return move (currentState);
    }

    if (location_.isA<AddressLocation> ())
    {
        // Print last address and it's value
        writeString (octalNumberToString (bus_->read (location_.address ())) + ' ');
        return AddressOpened_3 {};
    }

    if (location_.isA<RegisterLocation> ())
    {
        writeString (octalNumberToString (cpu_.r[location_.registerNr ()]) + ' ');
        return RegisterOpened_4 {};
    }

    if (location_.isA<PSWLocation> ())
    {
        writeString (octalNumberToString (cpu_.psw) + ' ');
        return RegisterOpened_4 {};
    }

    // Satisfy the compiler
    throw "Cannot happen";
}

State KD11ODT::transition (AtPrompt_1 &&, ProceedCmdEntered)
{
    return ExitPoint {};
}