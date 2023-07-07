#include "kd11odt.h"

using namespace KD11_ODT;

// This file contains the state transitions for the state EnteringAddress_5.
State KD11ODT::transition (EnteringAddress_5 &&, DigitEntered digitEntered)
{
    // Append the digit typed by the user to the address string and allow more
    // characters to be typed
    digitSeries_.push_back (digitEntered.digit);
    return EnteringAddress_5 {};
}

// While RUBOUT is not truly a command, the console does support
// this character. When typing in either address or data, the user can
// type RUBOUT to erase the previously typed character and the
// console will respond with a "\" (Backslash-ASCII 134) for every
// typed RUBOUT.
//
// Note that if RUBOUT is issued repeatedly, only numerical characters are
// erased and it is not possible to terminate the present mode the
// console is in. [This implies the EnteringAddress_5 state is not left.]
//
State KD11ODT::transition (EnteringAddress_5 &&currentState, RuboutEntered)
{
    // Remove the last entered character and echo a backslash. Prevent
    // undefined behaviour by checking if there is a character to pop.
    if (!digitSeries_.empty ())
        digitSeries_.pop_back ();

    console_->write ('\\');
    return move (currentState);
}

// In general, when the user is specifying an address or data, OOT will use
// the last eight digits if more than eight have been entered. The user need
// not enter leading Os for either address or data; OOT forces Os as the
// default. If an odd address is entered, the low-order bit is ignored, and a
// full 16-bit word is displayed. (Microcomputer and Memories Chapter 7)
//
// If an odd address is entered, the low-order bit is ignored, and a full
// 16-bit word is displayed. (EK-KDJ1A-UG-001)
//
// This is also the behaviour of the KDJ11-A processor. The KDJ11-B and
// KDJ11-E behave differently however: "If an odd address is entered, console
// ODT responds to the error by printing ?<CR><LF> or @". 
// (EK-KDJ1B-UG-001 and KDJ1E-UG-001)
//
State KD11ODT::transition (EnteringAddress_5 &&, OpenLocationCmdEntered)
{
    return move (openAddress ());
}

// Transform the now complete address string to an address and use it in
// The GO command. The address string is entered as on octal number.
// 
// Before starting execution, a BUS INIT is issued for 10 microseconds
// followed by 90 microseconds of idle time.
// (LSI11 PDP11/03 Processor Handbook)
//
// Note that a semicolon character (ASCII 073) can be used to separate the
// address from the G and this is done for PDP-11 ODT compatibility.
// (LSI11 PDP11/03 Processor Handbook)
//
// Recognition of the semicolon is not implemented as the reaction on
// input of characters after the semicolon is not described and is unclear.
//
State KD11ODT::transition (EnteringAddress_5 &&, GoCmdEntered)
{
    // A byte address can be specified as start address. Bit 0 of the start
    // address will be ignored and the CPU will start at the specified
    // address minus 1. When the CPU then halts bit 0 of the printed address
    // will be set so the printed address is the halt address plus 1.
    if (u16 address; stringTou16 (digitSeries_, 8, &address))
    {
        // Set the CPU into the running state with the specified address
        // as the PC and exit ODT
        startCPU (address);
        return ExitPoint {};
    }

    // Invalid address entered
    writeString ("?\n");
    return AtPrompt_1 {};
}