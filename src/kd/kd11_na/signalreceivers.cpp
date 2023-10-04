#include "kd11_na.h"

using std::bind;
using std::placeholders::_1;

// Get notifications on the state of the signals
void KD11_NA::subscribeToSignals ()
{
    bus_->BHALT().subscribe (bind (&KD11_NA::BHALTReceiver, this, _1));
    bus_->BPOK().subscribe (bind (&KD11_NA::BPOKReceiver, this, _1));
    bus_->RESET().subscribe (bind (&KD11_NA::ResetReceiver, this, _1));
}

// The BHALT signal halts the processor. 
void KD11_NA::BHALTReceiver (bool signalValue)
{
    if (signalValue)
        signalEventQueue_.push (Halt {});
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
void KD11_NA::BPOKReceiver (bool signalValue)
{
    if (signalValue)
        signalEventQueue_.push (BPOK_high {});
    else
        signalEventQueue_.push (BPOK_low {});
}

void KD11_NA::ExitReceiver (bool signalValue)
{
    signalEventQueue_.push (Exit {});
}

void KD11_NA::ResetReceiver (bool signalValue)
{
    if (signalValue)
        signalEventQueue_.push (Reset {});
}