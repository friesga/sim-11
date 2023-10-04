#include "kd11_na_odt.h"

#include <cassert>

using std::move;
using std::monostate;

KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (EnteringRegisterValue_8 &&, DigitEntered digitEntered)
{
    context_->digitSeries_.push_back (digitEntered.digit);
    return EnteringRegisterValue_8 {};
}

// See the comment at the equivalent EnteringAddressValue_7 state
KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (EnteringRegisterValue_8 &&currentState, RuboutEntered)
{
    // We expect there always is at least one character in the digitSeries_
    // buffer available. This either is a digit entered by the user or a '0'
    // in case the user has typed a RUBOUT in the AddressOpened_3 state.
    assert (!context_->digitSeries_.empty ());

    // Echo a backslash and remove the last entered character. If the 
    // digitSeries_ is now empty replace it with a '0'.
    context_->console_->write ('\\');
    context_->digitSeries_.pop_back ();
    if (context_->digitSeries_.empty ())
        context_->digitSeries_.push_back ('0');

    return move (currentState);
}

KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (EnteringRegisterValue_8 &&, CloseLocationCmdEntered)
{   
    context_->setRegisterValue ();
    context_->writeString ("\n");
    return AtPrompt_1 {};
}

// When the user enters an Open location command (/) the given value has to be
// used as an address to open.
KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (EnteringRegisterValue_8&&, OpenLocationCmdEntered)
{
    return move (context_->openAddress ());
}

KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (EnteringRegisterValue_8 &&, OpenNextLocationCmdEntered)
{
    context_->setRegisterValue ();
    return move (context_->openNextRegister (RegisterOpened_4 {},
        [this] () {return (context_->location_.registerNr () + 1) % 8;}));
}

KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (EnteringRegisterValue_8 &&, OpenPreviousLocationCmdEntered)
{
    context_->writeString ("\n");
    context_->setRegisterValue ();
    return move (context_->openNextRegister (RegisterOpened_4 {},
        [this] () {return static_cast<u8> (context_->location_.registerNr () - 1) % 8;}));
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
KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (EnteringRegisterValue_8 &&, AtSignCmdEntered)
{
    context_->writeString ("\n"); 
    context_->setRegisterValue ();
    u16 addressToOpen {0};
    if (context_->location_.isA<RegisterLocation> ())
        addressToOpen = context_->cpu_->registers () [context_->location_.registerNr ()];
    else
    {
        // The PSW is opened
        assert (context_->location_.isA<PSWLocation> ());
        
        // In case there is no currently open location print the error indication
        if (context_->location_.previousIsA<monostate> ())
        {
            context_->writeString ("@/?\n");
            return AtPrompt_1 {};
        }

        // In case the previously opened location was an
        // address, the address to open is that address; in case the
        // previously openend location was a register the contents of that
        // address is the location to open.
        if (context_->location_.previousIsA<AddressLocation> ())
            addressToOpen = context_->location_.previousInputAddress ();
        else
            addressToOpen = context_->cpu_->registers () [context_->location_.previousRegisterNr ()];
    }
    return context_->openNextAddress ([=] () {return addressToOpen;});
}

// Note that this [back arrow] command cannot be used if a GPR or the PS is
// the open location and if attempted, the command will modify the GPR
// or PS if data has been typed, and close the GPR or PS; then a CR,
// LF, @ will be issued. (LSI11 PDP11/03 Processor Handbook)
KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (EnteringRegisterValue_8 &&, BackArrowCmdEntered)
{   
    context_->setRegisterValue ();
    // The cursor is positioned just after the _, so write a new line
    // to position the cursor at the begin of the new line.
    context_->writeString ("\n");
    return AtPrompt_1 {};
}
