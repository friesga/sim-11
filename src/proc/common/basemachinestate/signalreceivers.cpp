#include "basemachinestate.h"

using std::bind;
using std::placeholders::_1;

// Get notifications on the state of the signals
void BaseMachineState::subscribeToSignals ()
{
    bus_->BHALT().subscribe (bind (&BaseMachineState::BHALTReceiver, this, _1));
    bus_->START ().subscribe (bind (&BaseMachineState::STARTReceiver, this, _1));
    bus_->BPOK().subscribe (bind (&BaseMachineState::BPOKReceiver, this, _1));
    bus_->RESET().subscribe (bind (&BaseMachineState::ResetReceiver, this, _1));
    bus_->BOOT ().subscribe (bind (&BaseMachineState::BootReceiver, this, _1));
}

// The BHALT signal halts the processor. 
void BaseMachineState::BHALTReceiver (bool signalValue)
{
    if (signalValue)
        signalEventQueue_.push (Halt {});
}

void BaseMachineState::STARTReceiver (bool signalValue)
{
    if (signalValue)
        signalEventQueue_.push (Start {});
}

// The BPOK signal triggers the procesor power-up routine
// 
// The reset is executed when either:
// - The system has been powered up,
// - The user hits the BREAK key and the boot response is configured.
//
// Note that this function will be executed in a different thread from the
// thread in which the CPU is running. Access to the signal event is 
// synchronized via the ThreadSafeQueue.
void BaseMachineState::BPOKReceiver (bool signalValue)
{
    if (signalValue)
        signalEventQueue_.push (BPOK_high {});
    else
        signalEventQueue_.push (BPOK_low {});
}

void BaseMachineState::ExitReceiver (bool signalValue)
{
    signalEventQueue_.push (Exit {});
}

void BaseMachineState::ResetReceiver (bool signalValue)
{
    if (signalValue)
        signalEventQueue_.push (Reset {});
}

void BaseMachineState::BootReceiver (bool signalValue)
{
    if (signalValue)
        signalEventQueue_.push (Boot {});
}