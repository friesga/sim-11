#include "kd11odt.h"
#include "trace/trace.h"

using namespace KD11_ODT;

using std::move;
using std::to_string;

// This file contains the entry actions and state transitions for
// the state AtPrompt_1.

void KD11ODT::entry (AtPrompt_1)
{
    console_->write ('@');
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
        // Print last opened address and it's value
        return writeAddressContents (location_.wordAddress ());
    }

    if (location_.isA<RegisterLocation> ())
    {
        writeString (octalNumberToString (cpu_.registerValue (location_.registerNr ())) + ' ');
        return RegisterOpened_4 {};
    }

    if (location_.isA<PSWLocation> ())
    {
        writeString (octalNumberToString (cpu_.pswValue ()) + ' ');
        return RegisterOpened_4 {};
    }

    // Satisfy the compiler
    throw "Cannot happen";
}

// On a Proceed command set the CPU into the running state and exit ODT
State KD11ODT::transition (AtPrompt_1 &&, ProceedCmdEntered)
{
    cpu_.proceed ();
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_P, cpu_.registerValue (7));
    return ExitPoint {};
}

// On a Go command start the CPU at the default address (000000).
State KD11ODT::transition (AtPrompt_1 &&, GoCmdEntered)
{
    startCPU (000000);
    return ExitPoint {};
}

// The "M" (Maintenance) command is used for maintenance purposes and prints
// the contents of an internal CPU register. This data reflects how the
// machine got to the console mode.
// The value is or'ed with 010 as a real LSI-11/2 prints that value.
State KD11ODT::transition (AtPrompt_1&&, MaintenanceCmdEntered)
{
    writeString (octalNumberToString (static_cast<u16> (cpu_.haltReason_) | 010) + '\n');
    return AtPrompt_1 {};
}