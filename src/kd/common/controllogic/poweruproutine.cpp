#include "controllogic.h"

// The reaction on a power-up is configured by the power-up mode. Three
// power-up modes can be selected:
// 1. Trap to location 24/26,
// 2. Place the processor in ODT mode,
// 3. Start the system at the boot address.
// 
// These modes can be selected in the KD11_NA section of the configuration file.
// See Table 11-4 (Console Power-Up Printout (or Display) in the LSI-11,
// PDP-11/03 user's manual (EK-LSI11-TM-003) for the expected reaction on
// power-up.
//
// During the power-up sequence, the processor asserts BINIT L in response to
// a passive (low) power supply-generated BDCOK H signal. When BDCOK H goes
// active (high), the processor terminates BINIT L and the jumper-selected
// power-up sequence is executed. (EK-LSI11-TM-003)
//
// The function will return the state to transition to.
//
ControlLogic::State ControlLogic::powerUpRoutine ()
{
    cpuControl_->cpuReset ();
    bus_->BINIT().cycle ();

    switch (powerUpMode_)
    {
        case KD11_NAConfig::PowerUpMode::Vector:
            cpu_->loadTrapVector (CpuData::TrapCondition::PowerFail);
            cpuControl_->proceed ();

            // If BHALT is set immediately transition to the Halted state,
            // before even one instruction is executed cf Table 11-4.
            if (bus_->BHALT ())
                return Halted {};

            return Running {};

        case KD11_NAConfig::PowerUpMode::ODT:
            // Halt the processor (if it isn't already halted). This will
            // place the processor in ODT mode on the next execution of
            // KD11_NA::step(). If the processor already is in ODT mode the
            // signal is ignored and this is a no-operation.
            cpuControl_->halt ();
            return Halted {};

        case KD11_NAConfig::PowerUpMode::Bootstrap:
            // Start the processor at the start address. This address is
            // either the standard boot address or an address determined by
            // a loaded file in absolute loader format.
            cpuControl_->start (startAddress_);

            // If BHALT is set immediately transition to the Halted state,
            // before even one instruction is executed cf Table 11-4.
            if (bus_->BHALT ())
                return Halted {};

            return Running {};
    }

    // Satisfying the compiler
    throw string ("Unknown PowerUpMode");
}
