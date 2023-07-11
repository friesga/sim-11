#include "kd11.h"

using namespace kd11_na;
using namespace KD11_ODT;

using std::make_unique;
using std::monostate;

kd11_na::State KD11::transition (PowerOff&&, BPOK_high)
{
    return powerUpRoutine ();
}

void KD11::entry (Running)
{
    while (!signalAvailable ())
    {
        // Increment the bus delay timer. This really should be solved
        // otherwise, but as the CPU steps are performed in this fumction
        // the bus steps have to be executed here too.
        bus_->step ();

        // If the CPU is halted or BHALT is true ODT must be started. In the
        // latter case one instruction is executed and thus the CPU is single
        // stepped.
        if (!cpu_.step () || bus_->BHALT())
            signalEventQueue_.push (Halt {});
    }
    bus_->SRUN ().set (false);
}

kd11_na::State KD11::transition (Running&&, Reset)
{
    return powerUpRoutine ();
}

kd11_na::State KD11::transition (Running&&, Halt)
{
    return Halted {};
}


kd11_na::State KD11::transition (Running&&, BPOK_low)
{
    return PowerFail {};
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

kd11_na::State KD11::transition (Halted&&, Start)
{
    return Running {};
}

kd11_na::State KD11::transition (Halted&&, Reset)
{
    return powerUpRoutine ();
}
 
kd11_na::State KD11::transition (Halted&&, BPOK_low)
{
    return PowerOff {};
}

// Execute the powerfail routine until either a HALT is executed
// or the DC runs out. The latter is simulated by executing a maximum of
// 1000 instructions (presuming an avering instruction exeution time of
// 4 microseconds and 4 milliseconds DC power available). The powerfail
// routine cannot be single stepped.
void KD11::entry (PowerFail)
{
    size_t maxInstructions {1000};

    // On a powerfail trap to the vector at address 24/26
    cpu_.setTrap (&powerFail);

    while (!signalAvailable () && maxInstructions-- > 0)
    {
        if (!cpu_.step ())
        {
            signalEventQueue_.push (Halt {});
            return;
        }
    }

    if (maxInstructions == 0)
        signalEventQueue_.push (BDCOK_low {});
}

kd11_na::State KD11::transition (PowerFail&&, BDCOK_low)
{
    bus_->SRUN ().set (false);
    return PowerOff {};
}

kd11_na::State KD11::transition (PowerFail&&, Halt)
{
    bus_->SRUN ().set (false);
    return PowerOff {};
}

void KD11::entry (ExitPoint)
{
    kd11Running_ = false;
}