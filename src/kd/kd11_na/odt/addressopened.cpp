#include "kd11_na_odt.h"

KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (AddressOpened_3 &&, DigitEntered digitEntered)
{
    context_->digitSeries_ = digitEntered.digit;
    return EnteringAddressValue_7 {};
}

// A RUBOUT is entered while there is no digit for the new value entered.
// Echo a backslash character, put a '0' in the digitSeries_ buffer and
// transition to the EnteringAddressValue_7 state. This way the default
// new value will be zero when more characters are erased than are available
// in the buffer. See the comment at the RuboutEntered transition in the
// EnteringAddressValue_7 state.
KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (AddressOpened_3 &&, RuboutEntered)
{
    context_->console_->write ('\\');
    context_->digitSeries_ = '0';
    return EnteringAddressValue_7 {};
}

// This file contains the state transitions for the state AddressOpened_3.
KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (AddressOpened_3 &&, CloseLocationCmdEntered)
{
    context_->writeString ("\n");
    return AtPrompt_1 {};
}

// This [open next location] command is also used to close an open location
// or GPR (general purpose register). If entered after a location has been
// opened, it will close the open location or GPR and open location + 2 or
// GPR + l. If the contents of the open location or GPR are
// to be modified, the new contents should precede the LF operator.
// (Microcomputer and Memories Chapter 7)
KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (AddressOpened_3 &&, OpenNextLocationCmdEntered)
{
    return context_->openNextAddress ([this] () {return context_->location_.inputAddress () + 2;});
}

// The "up arrow" command is also used to close an open location or GPR.
// If entered after a location or GPR has been opened, it will close
// the open location or GPR and open location -2, or GPR-1. If the
// contents of the open location or GPR are to be modified, the new
// contents should precede the "up arrow" operator.
// (Microcomputer and Memories Chapter 7)
KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (AddressOpened_3 &&, OpenPreviousLocationCmdEntered)
{
    context_->writeString ("\n");
    return context_->openNextAddress ([this] () {return context_->location_.inputAddress () - 2;});
}

// Once a location has been opened, the @ command is used to close
// that location and open a second location, using the contents of the first
// location as an indirect address to the second location. That is, the
// contents of the first location point to the second location to be opened.
// The contents of the first location can be modified before the @ command
// is used. (Microcomputer and Memories Chapter 7)
KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (AddressOpened_3 &&, AtSignCmdEntered)
{
    context_->writeString ("\n");
    return context_->openNextAddress ([this] ()
        {return context_->bus_->read (BusAddress<16> (context_->location_.inputAddress ()));});
}

// This command is used once a location has been opened to open
// the location that is the address of the contents of the open location
// plus the address of the open location plus 2. 
// (LSI11 PDP11/03 Processor Handbook)
//
// ODT interprets the contents of the currently open *word* as an address
// indexed by the PC and opens the addressed location. (MicroProcessor
// Computers Handbook).
// The last sentence seems to indicate the word address has to be used
// instead of the input address (which might be a byte address).
 KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (AddressOpened_3 &&, BackArrowCmdEntered)
 {
    context_->writeString ("\n");
    return context_->openNextAddress ([this] () 
        {return context_->location_.wordAddress () + 
            context_->bus_->read (BusAddress<16> (context_->location_.wordAddress ())) + 2;});
 }

// Micronote 050 (Micro ODT Differences - LSI-11 vs. LSI-11/23) states that
// when an address location is open, another location can be opened without
// explicitly closing the first location. Tests conducted at a real LSi-11/2
// show that the same holds for register locations, i.e. when an address
// location is open a register location can be opened and when a register
// location is open an address or register location can be opened.
KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (AddressOpened_3&&, RegisterCmdEntered)
{
    return StartingRegister_2 {};
}