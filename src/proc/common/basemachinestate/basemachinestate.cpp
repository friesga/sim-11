#include "basemachinestate.h"
#include "bus/qbus/qbus.h"
#include "proc/kd/common/odt/operatorconsoleaccess/operatorconsoleaccess.h"

#include <memory>
#include <thread>
#include <chrono>
#include <string>
#include <iostream>

using std::make_unique;
using std::cerr;

// The factory power-up mode configuration is mode 0 (get vector at address
// 24 and 26), but we'll set it to Bootstrap as that's more convenient for
// the user.
BaseMachineState::BaseMachineState (Bus* bus, CpuData* cpuData,
    CpuControl* cpuControl, MMU* mmu)
    :
    bus_ (bus),
    cpuData_ {cpuData},
    cpuControl_ {cpuControl},
    mmu_ {mmu},
    running_ {true}
{
    stateMachine_ = make_unique<StateMachine> (this);
    subscribeToSignals ();
}

// Run the MachineState state machine
void BaseMachineState::run ()
try
{
    Event event;

    while (running_)
    {
        // Read a character from the console, create the appropriate event
        // from it and process that event
        signalEventQueue_.waitAndPop (event);
        stateMachine_->dispatch (event);
    }
}
catch (const std::exception& ex)
{
    cerr << "MachineState::run exception: " << ex.what () << '\n';
}

void BaseMachineState::exit ()
{
    signalEventQueue_.push (Exit {});
}

bool BaseMachineState::signalAvailable ()
{
    return signalEventQueue_.size () > 0;
}

// Load PC and PSW from the given vector
void BaseMachineState::loadTrapVector (CpuData::TrapType trap)
{
    cpuData_->registers ()[7] = mmu_->fetchWord (cpuData_->trapVector (trap)).valueOr (0);
    cpuData_->psw ().set (PSW::ProtectionMode::Trap,
        mmu_->fetchWord (cpuData_->trapVector (trap) + 2).valueOr (0));
}