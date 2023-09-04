#include "kd11odt.h"

using namespace KD11_ODT;

#include <cassert>

using std::move;
using std::monostate;

State KD11ODT::transition (EnteringRegisterValue_8 &&, DigitEntered digitEntered)
{
    digitSeries_.push_back (digitEntered.digit);
    return EnteringRegisterValue_8 {};
}

// See the comment at the equivalent EnteringAddressValue_7 state
State KD11ODT::transition (EnteringRegisterValue_8 &&currentState, RuboutEntered)
{
    // We expect there always is at least one character in the digitSeries_
    // buffer available. This either is a digit entered by the user or a '0'
    // in case the user has typed a RUBOUT in the AddressOpened_3 state.
    assert (!digitSeries_.empty ());

    // Echo a backslash and remove the last entered character. If the 
    // digitSeries_ is now empty replace it with a '0'.
    console_->write ('\\');
    digitSeries_.pop_back ();
    if (digitSeries_.empty ())
        digitSeries_.push_back ('0');

    return move (currentState);
}

State KD11ODT::transition (EnteringRegisterValue_8 &&, CloseLocationCmdEntered)
{   
    setRegisterValue ();
    writeString ("\n");
    return AtPrompt_1 {};
}

// When the user enters an Open location command (/) the given value has to be
// used as an address to open.
State KD11ODT::transition (EnteringRegisterValue_8&&, OpenLocationCmdEntered)
{
    return move (openAddress ());
}

State KD11ODT::transition (EnteringRegisterValue_8 &&, OpenNextLocationCmdEntered)
{
    setRegisterValue ();
    return move (openNextRegister (RegisterOpened_4 {},
        [this] () {return (location_.registerNr () + 1) % 8;}));
}

State KD11ODT::transition (EnteringRegisterValue_8 &&, OpenPreviousLocationCmdEntered)
{
    writeString ("\n");
    setRegisterValue ();
    return move (openNextRegister (RegisterOpened_4 {},
        [this] () {return static_cast<u8> (location_.registerNr () - 1) % 8;}));
}

// If used on the PS, the command will modify the PS if data has
// been typed and close it; however, the last GPR or memory location
// contents will be used as a pointer. (LSI11 PDP11/03 Processor Handbook)
// 
// Test runs on a real LSI-11/2 show that the last sentence has to be read
// as: "the last GPR contents or memory location contents will be used as a
// pointer".
// 
// In case there is no previously opened location an error is retuned on
// opening the location.
State KD11ODT::transition (EnteringRegisterValue_8 &&, AtSignCmdEntered)
{
    writeString ("\n"); 
    setRegisterValue ();
    u16 addressToOpen {0};
    if (location_.isA<RegisterLocation> ())
        addressToOpen = cpu_.register_[location_.registerNr ()];
    else
    {
        // The PSW is opened
        assert (location_.isA<PSWLocation> ());
        
        // In case there is no currently open location print the error indication
        if (location_.previousIsA<monostate> ())
        {
            writeString ("@/?\n");
            return AtPrompt_1 {};
        }

        // In case the previously opened location was an
        // address, the address to open is that address; in case the
        // previously openend location was a register the contents of that
        // address is the location to open.
        if (location_.previousIsA<AddressLocation> ())
            addressToOpen = location_.previousInputAddress ();
        else
            addressToOpen = cpu_.register_[location_.previousRegisterNr ()];
    }
    return openNextAddress ([=] () {return addressToOpen;});
}

// Note that this [back arrow] command cannot be used if a GPR or the PS is
// the open location and if attempted, the command will modify the GPR
// or PS if data has been typed, and close the GPR or PS; then a CR,
// LF, @ will be issued. (LSI11 PDP11/03 Processor Handbook)
State KD11ODT::transition (EnteringRegisterValue_8 &&, BackArrowCmdEntered)
{   
    setRegisterValue ();
    // The cursor is positioned just after the _, so write a new line
    // to position the cursor at the begin of the new line.
    writeString ("\n");
    return AtPrompt_1 {};
}