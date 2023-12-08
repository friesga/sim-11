#include "controllogic.h"
#include "qbus/qbus.h"
#include "kd/kd11_na/operatorconsoleaccess/operatorconsoleaccess.h"

#include <memory>
#include <thread>
#include <chrono>
#include <string>

using std::shared_ptr;
using std::make_unique;

// The factory power-up mode configuration is mode 0 (get vector at address
// 24 and 26), but we'll set it to Bootstrap as that's more convenient for
// the user.
ControlLogic::ControlLogic (Qbus* bus, CpuData* cpu,
    CpuControl* cpuControl, MMU* mmu,
    KD11_NAConfig::PowerUpMode powerUpMode, u16 startAddress,
    KD11ODT::Creator odtCreator)
    :
    bus_ (bus),
    cpu_ {cpu},
    cpuControl_ {cpuControl},
    mmu_ {mmu},
    powerUpMode_ {powerUpMode},
    startAddress_ {startAddress},
    odtCreator_ {odtCreator},
    kd11Running_ {true}
{
    stateMachine_ = make_unique<StateMachine> (this);
    subscribeToSignals ();
}

// Run the ControlLogic state machine
void ControlLogic::run ()
{
    Event event;

    while (kd11Running_)
    {
        // Read a character from the console, create the appropriate event
        // from it and process that event
        signalEventQueue_.waitAndPop (event);
        stateMachine_->dispatch (event);
    }
}

void ControlLogic::exit ()
{
    signalEventQueue_.push (Exit {});
}

bool ControlLogic::signalAvailable ()
{
    return signalEventQueue_.size () > 0;
}

// Load PC and PSW from the given vector
void ControlLogic::loadTrapVector (CpuData::TrapCondition trap)
{
    cpu_->registers ()[7] = mmu_->fetchWord (cpu_->trapVector (trap)).valueOr (0);
    cpu_->loadPSW (mmu_->fetchWord (cpu_->trapVector (trap) + 2).valueOr (0));
}