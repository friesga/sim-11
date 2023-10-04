#include "kd11_na.h"

using std::make_unique;
using std::monostate;

KD11_NA::State KD11_NA::StateMachine::transition (PowerOff&&, BPOK_high)
{
    return context_->powerUpRoutine ();
}

void KD11_NA::StateMachine::entry (Running)
{
    while (!context_->signalAvailable ())
    {
        // Increment the bus delay timer. This really should be solved
        // otherwise, but as the CPU steps are performed in this fumction
        // the bus steps have to be executed here too.
        context_->bus_->step ();

        // If the CPU is halted or BHALT is true ODT must be started. In the
        // latter case one instruction is executed and thus the CPU is single
        // stepped.
        if (!context_->cpu_.step () || context_->bus_->BHALT())
            context_->signalEventQueue_.push (Halt {});
    }
    context_->bus_->SRUN ().set (false);
}

KD11_NA::State KD11_NA::StateMachine::transition (Running&&, Reset)
{
    return context_->powerUpRoutine ();
}

KD11_NA::State KD11_NA::StateMachine::transition (Running&&, Halt)
{
    return Halted {};
}


KD11_NA::State KD11_NA::StateMachine::transition (Running&&, BPOK_low)
{
    return PowerFail {};
}

// On every entry to ODT a new KD11_NA_ODT object is created to make
// sure it is initialized properly. The Microcomputer and Memories
// Handbook states: "A / issued immediately after the processor
// enters ODT mode causes a ?<CR><LF> to be printed because a
// location has not yet been opened.
void KD11_NA::StateMachine::entry (Halted)
{
    context_->runODT ();
}

KD11_NA::State KD11_NA::StateMachine::transition (Halted&&, Start)
{
    return Running {};
}

KD11_NA::State KD11_NA::StateMachine::transition (Halted&&, Reset)
{
    return context_->powerUpRoutine ();
}
 
KD11_NA::State KD11_NA::StateMachine::transition (Halted&&, BPOK_low)
{
    return PowerOff {};
}

// Execute the powerfail routine until either a HALT is executed
// or the DC runs out. The latter is simulated by executing a maximum of
// 1000 instructions (presuming an avering instruction exeution time of
// 4 microseconds and 4 milliseconds DC power available). The powerfail
// routine cannot be single stepped.
void KD11_NA::StateMachine::entry (PowerFail)
{
    size_t maxInstructions {1000};

    // On a powerfail trap to the vector at address 24/26
    context_->cpu_.setTrap (&context_->powerFail);

    while (!context_->signalAvailable () && maxInstructions-- > 0)
    {
        if (!context_->cpu_.step ())
        {
            context_->signalEventQueue_.push (Halt {});
            return;
        }
    }

    if (maxInstructions == 0)
        context_->signalEventQueue_.push (BDCOK_low {});
}

KD11_NA::State KD11_NA::StateMachine::transition (PowerFail&&, BDCOK_low)
{
    context_->bus_->SRUN ().set (false);
    return PowerOff {};
}

KD11_NA::State KD11_NA::StateMachine::transition (PowerFail&&, Halt)
{
    context_->bus_->SRUN ().set (false);
    return PowerOff {};
}

void KD11_NA::StateMachine::entry (ExitPoint)
{
    context_->kd11Running_ = false;
}