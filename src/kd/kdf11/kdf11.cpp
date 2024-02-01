#include "kdf11.h"
#include "qbus/qbus.h"

#include <memory>
#include <thread>

using std::shared_ptr;
using std::make_unique;
using std::thread;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::placeholders::_5;

KDF11::KDF11 (Qbus* bus, KD11Config::PowerUpMode powerUpMode, u16 startAddress)
    :
    bus_ {bus},
    powerUpMode_ {powerUpMode_},
    startAddress_ {startAddress}
{}

KDF11::~KDF11 ()
{
    controlLogic_->exit ();
    kd11Thread_.join ();
}

void KDF11::start ()
{
    kd11Thread_ = thread ([&, this] {controlLogic_->run ();});
}

// Start the ControlLogic state machine, starting the CPU at the given address. This
// address supersedes the standard boot address.
void KDF11::start (u16 startAddress)
{
    startAddress_ = startAddress;
    start ();
}

StatusCode KDF11::read (BusAddress address, u16* destination)
{
    for (BusDevice* module : cpuModules_)
        if (module->responsible (address))
            return module->read (address, destination);

    return StatusCode::NonExistingMemory;
}

StatusCode KDF11::writeWord (BusAddress address, u16 value)
{
    for (BusDevice* module : cpuModules_)
        if (module->responsible (address))
            return module->writeWord (address, value);

    return StatusCode::NonExistingMemory;

}

bool KDF11::responsible (BusAddress busAddress)
{
    if (!busAddress.isInIOpage ())
        return false;

    for (BusDevice* module : cpuModules_)
        if (module->responsible (busAddress))
            return true;

    return false;
}

void KDF11::reset ()
{
    for (BusDevice* module : cpuModules_)
        module->reset ();
}   
