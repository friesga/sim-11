#include "controllogic.h"

using std::make_unique;
using std::monostate;

ControlLogic::State ControlLogic::StateMachine::transition (PowerOff&&, BPOK_high)
{
    return context_->powerUpRoutine ();
}

ControlLogic::State ControlLogic::StateMachine::transition (Standby&&, BPOK_high)
{
    return context_->powerUpRoutine ();
}

void ControlLogic::StateMachine::entry (Running)
{
    while (!context_->signalAvailable ())
    {
        // If the CPU is halted or BHALT is true ODT must be started. In the
        // latter case one instruction is executed and thus the CPU is single
        // stepped.
        if (!context_->cpuControl_->step () || context_->bus_->BHALT() || 
            context_->cpuControl_->inHaltMode ())
                context_->signalEventQueue_.push (Halt {});
    }
    context_->bus_->SRUN ().set (false);
}

ControlLogic::State ControlLogic::StateMachine::transition (Running&&, Reset)
{
    return context_->powerUpRoutine ();
}

ControlLogic::State ControlLogic::StateMachine::transition (Running&&, Boot)
{
    return context_->bootRoutine ();
}

ControlLogic::State ControlLogic::StateMachine::transition (Running&&, Halt)
{
    return Halted {};
}

ControlLogic::State ControlLogic::StateMachine::transition (Running&&, BPOK_low)
{
    return PowerFail {};
}

// On every entry to ODT a new KD11_NA_ODT object is created to make
// sure it is initialized properly. The Microcomputer and Memories
// Handbook states: "A / issued immediately after the processor
// enters ODT mode causes a ?<CR><LF> to be printed because a
// location has not yet been opened.
void ControlLogic::StateMachine::entry (Halted)
{
    context_->runODT ();
}

ControlLogic::State ControlLogic::StateMachine::transition (Halted&&, Start)
{
    return Running {};
}

ControlLogic::State ControlLogic::StateMachine::transition (Halted&&, Reset)
{
    return context_->powerUpRoutine ();
}

ControlLogic::State ControlLogic::StateMachine::transition (Halted&&, Boot)
{
    return context_->bootRoutine ();
}
 
ControlLogic::State ControlLogic::StateMachine::transition (Halted&&, BPOK_low)
{
    return PowerOff {};
}

// Execute the powerfail routine until either a HALT is executed
// or the DC runs out. The latter is simulated by executing a maximum of
// 1000 instructions (presuming an avering instruction exeution time of
// 4 microseconds and 4 milliseconds DC power available). The powerfail
// routine cannot be single stepped.
void ControlLogic::StateMachine::entry (PowerFail)
{
    size_t maxInstructions {1000};

    // On a powerfail trap to the vector at address 24/26
    context_->cpuData_->setTrap (CpuData::TrapCondition::PowerFail);

    while (!context_->signalAvailable () && maxInstructions-- > 0)
    {
        if (!context_->cpuControl_->step ())
        {
            context_->signalEventQueue_.push (Halt {});
            return;
        }
    }

    if (maxInstructions == 0)
        context_->signalEventQueue_.push (BDCOK_low {});
}

ControlLogic::State ControlLogic::StateMachine::transition (PowerFail&&, BDCOK_low)
{
    return context_->powerDownRoutine ();
}

ControlLogic::State ControlLogic::StateMachine::transition (PowerFail&&, Halt)
{
    return context_->powerDownRoutine ();
}

void ControlLogic::StateMachine::entry (ExitPoint)
{
    context_->kd11Running_ = false;
}