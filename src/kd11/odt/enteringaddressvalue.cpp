#include "kd11odt.h"

using namespace KD11_ODT;

#include <cassert>

// This file contains the entry actions and state transitions for
// the state EnteringAddressValue_7.
State KD11ODT::transition (EnteringAddressValue_7 &&, DigitEntered digitEntered)
{ 
    digitSeries_.push_back (digitEntered.digit);
    return EnteringAddressValue_7 {};
}

// If more than six RUBOUTS are consecutively typed, and then a valid location
// closing command is typed, the open location will be modified with all
// zeroes. (LSI11 PDP11/03 Processor Handbook)
//
// I presume the last remark means that when more RUBOUTS are entered then
// characters are available in the buffer (digitSeries_), the open location
// will be set to zero. In this case we replace the last character by a
// zero. This is an ugly solution as the value typed in by the user now has
// a leading zero, but making a distinction between an empty string indicating
// there are no characters to be converted to a value and an empty string
// which should be converted to zero will create an even uglier solution.
State KD11ODT::transition (EnteringAddressValue_7 &&currentState, RuboutEntered)
{
    // We expect there always is at least one character in the digitSeries_
    // buffer available. This either is a digit entered by the user or a '0'
    // in case the user has typed a RUBOUT in the AddressOpened_3 state.
    assert (!digitSeries_.empty ());

    // Echo a backslash and remove the last entered character. If the 
    // digitSeries_ is now empty replace it with a '0'.
    character.write ('\\');
    digitSeries_.pop_back ();
    if (digitSeries_.empty ())
        digitSeries_.push_back ('0');

    return move (currentState);
}

State KD11ODT::transition (EnteringAddressValue_7 &&, CloseLocationCmdEntered)
{
    setAddressValue ();
    return AtPrompt_1 {};
}

State KD11ODT::transition (EnteringAddressValue_7 &&, OpenNextLocationCmdEntered)
{
    setAddressValue ();
    return openNextAddress ([this] () {return location_.address () + 2;});
}

State KD11ODT::transition (EnteringAddressValue_7 &&, OpenPreviousLocationCmdEntered)
{
    setAddressValue ();
    return openNextAddress ([this] () {return location_.address () - 2;});
}

// Also the open location can be optionally modified similar to other commands
// and if done, the new contents will be used as the pointer.
// (LSI11 PDP11/03 Processor Handbook)
State KD11ODT::transition (EnteringAddressValue_7 &&, AtSignCmdEntered)
{
    setAddressValue ();
    return openNextAddress ([this] () {return newValue_;});
}

State KD11ODT::transition (EnteringAddressValue_7 &&, BackArrowCmdEntered)
{
    setAddressValue ();
    return openNextAddress ([this] () 
        {return location_.address () + bus_->read (location_.address ()) + 2;});
}

// When an address location is open, another location can be opened without
// explicitly closing the first location; e.g., 1000/123456 2000/054321.
// (Micronote 050 Micro ODT Differences - LSI-11 vs. LSI-11/23)
State KD11ODT::transition (EnteringAddressValue_7 &&, OpenLocationCmdEntered)
{
    return move (openAddress ());
}