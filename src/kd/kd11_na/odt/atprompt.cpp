#include "kd11_na_odt.h"
#include "trace/trace.h"

#include <stdexcept>

using std::move;
using std::to_string;
using std::runtime_error;

// This file contains the entry actions and state transitions for
// the state AtPrompt_1.

void KD11_NA_ODT::StateMachine::entry (AtPrompt_1)
{
    context_->console_->write ('@');
}

KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (AtPrompt_1 &&, DigitEntered digitEntered)
{
    // The given digit is the first digit of an address
    context_->digitSeries_ = digitEntered.digit;
    return EnteringAddress_5 {};
}

KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (AtPrompt_1 &&, RegisterCmdEntered)
{
    context_->registerSeries_.clear ();
    return StartingRegister_2 {};
}

// The / command can be used without a location specifier to verify the data
// just entered into a previously opened location. The / produces this result
// only if it is entered immediately after a prompt character. A / issued
// immediately after the processor enters OOT mode will cause ? <CR>, <LF> to
// be printed because a location has not yet been opened. (EK-KDJ1A-UG-001)
KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (AtPrompt_1 && currentState, OpenLocationCmdEntered)
{
    if (context_->location_.isA<monostate> ())
    {
        // No location openend yet
        context_->writeString ("?\n");
        return move (currentState);
    }

    if (context_->location_.isA<AddressLocation<u16>> ())
    {
        // Print last opened address and it's value
        return context_->writeAddressContents (context_->location_.wordAddress ());
    }

    if (context_->location_.isA<RegisterLocation> ())
    {
        context_->writeString (context_->octalNumberToString (context_->cpuData_->registers () [context_->location_.registerNr ()]) + ' ');
        return RegisterOpened_4 {};
    }

    if (context_->location_.isA<PSWLocation> ())
    {
        context_->writeString (context_->octalNumberToString (context_->cpuData_->psw ()) + ' ');
        return RegisterOpened_4 {};
    }

    // Satisfy the compiler
    throw runtime_error ("Cannot happen");
}

// On a Proceed command set the CPU into the running state and exit ODT
KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (AtPrompt_1 &&, ProceedCmdEntered)
{
    context_->cpuControl_->proceed ();
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_P, context_->cpuData_->registers ()[7]);
    return ExitPoint {};
}

// On a Go command start the CPU at the default address (000000).
KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (AtPrompt_1 &&, GoCmdEntered)
{
    context_->startCPU (000000);
    return ExitPoint {};
}

// The "M" (Maintenance) command is used for maintenance purposes and prints
// the contents of an internal CPU register. This data reflects how the
// machine got to the console mode.
// The value is or'ed with 010 as a real LSI-11/2 prints that value.
KD11_NA_ODT::State KD11_NA_ODT::StateMachine::transition (AtPrompt_1&&, MaintenanceCmdEntered)
{
    context_->writeString (context_->octalNumberToString (static_cast<u16> (context_->cpuControl_->haltReason ()) | 010) + '\n');
    return AtPrompt_1 {};
}