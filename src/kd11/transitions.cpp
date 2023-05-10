#include "kd11.h"

using namespace kd11_f;
using namespace KD11_ODT;

using std::make_unique;

kd11_f::State KD11::transition (PowerOff&&, PowerOk)
{
    return powerUpRoutine ();
}

void KD11::entry (Running)
{
    while (!signalAvailable ())
    {
        // If the CPU is halted or BHALT is true ODT must be started. In the
        // latter case one instruction is executed and thus the CPU is single
        // stepped.
        if (!cpu_.step () || bus_->BHALT())
            signalEventQueue_.push (Halt {});
    }
    bus_->SRUN ().set (false);
}

kd11_f::State KD11::transition (Running&&, Reset)
{
    return powerUpRoutine ();
}

kd11_f::State KD11::transition (Running&&, Halt)
{
    return Halted {};
}

// On every entry to ODT a new KD11ODT object is created to make
// sure it is initialized properly. The Microcomputer and Memories
// Handbook states: "A / issued immediately after the processor
// enters ODT mode causes a ?<CR><LF> to be printed because a
// location has not yet been opened.
void KD11::entry (Halted)
{
    runODT ();
}

kd11_f::State KD11::transition (Halted&&, Start)
{
    return Running {};
}

kd11_f::State KD11::transition (Halted&&, Reset)
{
    return Running {};
}

void KD11::entry (ExitPoint)
{
    kd11Running_ = false;
}