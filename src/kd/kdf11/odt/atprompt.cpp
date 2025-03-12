#include "kdf11_odt.h"
#include "trace/trace.h"

#include <stdexcept>

using std::move;
using std::to_string;
using std::runtime_error;

// This file contains the entry actions and state transitions for
// the state AtPrompt_1.

void KDF11_ODT::StateMachine::entry (AtPrompt_1)
{
    context_->console_->write ('@');
}

KDF11_ODT::State KDF11_ODT::StateMachine::transition (AtPrompt_1 &&, DigitEntered digitEntered)
{
    // The given digit is the first digit of an address
    context_->digitSeries_ = digitEntered.digit;
    return EnteringAddress_5 {};
}

KDF11_ODT::State KDF11_ODT::StateMachine::transition (AtPrompt_1 &&, RegisterCmdEntered)
{
    context_->registerSeries_.clear ();
    return StartingRegister_2 {};
}

// The / command can be used without a location specifier to verify the data
// just entered into a previously opened location. The / produces this result
// only if it is entered immediately after a prompt character. A / issued
// immediately after the processor enters OOT mode will cause ? <CR>, <LF> to
// be printed because a location has not yet been opened. (EK-KDJ1A-UG-001)
KDF11_ODT::State KDF11_ODT::StateMachine::transition (AtPrompt_1 && currentState, OpenLocationCmdEntered)
{
    if (context_->location_.isA<monostate> ())
    {
        // No location openend yet
        context_->writeString ("?\n");
        return move (currentState);
    }

    if (context_->location_.isA<AddressLocation<u32>> ())
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
KDF11_ODT::State KDF11_ODT::StateMachine::transition (AtPrompt_1 &&, ProceedCmdEntered)
{
    context_->cpuControl_->proceed ();
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_P, context_->cpuData_->registers ()[7]);
    return ExitPoint {};
}

// On a Go command start the CPU at the default address (000000).
KDF11_ODT::State KDF11_ODT::StateMachine::transition (AtPrompt_1 &&, GoCmdEntered)
{
    context_->startCPU (000000);
    return ExitPoint {};
}

// ODT in the KDF11-U implements the "Toggle Halt" command:
// The CPU may be single stepped by a second method.Instead of placing the
// HALT/CONT/BOOT switch in the HALT position, the user can type an H on the
// console terminal. This action has the effect of toggling the halt flip-flop
// located in the CPU.This action is the same as that done by setting the
// HALT/CONT/BOOT switch to the HALT position. The CPU may now be single stepped
// by using the G and P commands, previously discussed.When exiting the
// single-step mode, type H and the halt flip-flop will be cleared.
// (EK-11024-TM-001, p. 3-9).
// 
// On the KDF11-A and KDF11-B this is a reserved command: An ASCII H (110) is
// reserved for future use by Digital. If it is accidently typed, ODT will
// echo the H and print a prompt character rayther than a ", which is the
// invalid character response. No other operation is performed.
// (Ek-KDF11-UG-PR2, par 3.4.9 and EK-KDFEB-UG-001 par. 3.5.9)
// 
// The behaviour of the flip-flop is implemented by the CpuControl halt mode.
// 
KDF11_ODT::State KDF11_ODT::StateMachine::transition (AtPrompt_1&&, HaltCmdEntered)
{
    if (context_->haltCmdSupported_)
        context_->cpuControl_->setHaltMode (context_->cpuControl_->inHaltMode () ? false : true);
    context_->writeString ("\n");
    return AtPrompt_1 {};
}
