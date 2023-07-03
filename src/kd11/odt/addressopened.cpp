#include "kd11odt.h"

using namespace KD11_ODT;

State KD11ODT::transition (AddressOpened_3 &&, DigitEntered digitEntered)
{
    digitSeries_ = digitEntered.digit;
    return EnteringAddressValue_7 {};
}

// A RUBOUT is entered while there is no digit for the new value entered.
// Echo a backslash character, put a '0' in the digitSeries_ buffer and
// transition to the EnteringAddressValue_7 state. This way the default
// new value will be zero when more characters are erased than are available
// in the buffer. See the comment at the RuboutEntered transition in the
// EnteringAddressValue_7 state.
State KD11ODT::transition (AddressOpened_3 &&, RuboutEntered)
{
    console_->write ('\\');
    digitSeries_ = '0';
    return EnteringAddressValue_7 {};
}

// This file contains the state transitions for the state AddressOpened_3.
State KD11ODT::transition (AddressOpened_3 &&, CloseLocationCmdEntered)
{
    console_->write ('\n');
    return AtPrompt_1 {};
}

// This [open next location] command is also used to close an open location
// or GPR (general purpose register). If entered after a location has been
// opened, it will close the open location or GPR and open location + 2 or
// GPR + l. If the contents of the open location or GPR are
// to be modified, the new contents should precede the LF operator.
// (Microcomputer and Memories Chapter 7)
State KD11ODT::transition (AddressOpened_3 &&, OpenNextLocationCmdEntered)
{
    return openNextAddress ([this] () {return location_.address () + 2;});
}

// The "up arrow" command is also used to close an open location or GPR.
// If entered after a location or GPR has been opened, it will close
// the open location or GPR and open location -2, or GPR-1. If the
// contents of the open location or GPR are to be modified, the new
// contents should precede the "up arrow" operator.
// (Microcomputer and Memories Chapter 7)
State KD11ODT::transition (AddressOpened_3 &&, OpenPreviousLocationCmdEntered)
{
    console_->write ('\n');
    return openNextAddress ([this] () {return location_.address () - 2;});
}

// Once a location has been opened, the @ command is used to close
// that location and open a second location, using the contents of the first
// location as an indirect address to the second location. That is, the
// contents of the first location point to the second location to be opened.
// The contents of the first location can be modified before the @ command
// is used. (Microcomputer and Memories Chapter 7)
State KD11ODT::transition (AddressOpened_3 &&, AtSignCmdEntered)
{
    console_->write ('\n');
    return openNextAddress ([this] () {return bus_->read (location_.address ());});
}

// This command is used once a location has been opened to open
// the location that is the address of the contents of the open location
// plus the address of the open location plus 2. 
// (LSI11 PDP11/03 Processor Handbook)
 State KD11ODT::transition (AddressOpened_3 &&, BackArrowCmdEntered)
 {
    console_->write ('\n');
    return openNextAddress ([this] () 
        {return location_.address () + bus_->read (location_.address ()) + 2;});
 }

