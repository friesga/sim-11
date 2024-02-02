#include "kdf11_a.h"
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

// The factory power-up mode configuration is mode 0 (get vector at address
// 24 and 26), but we'll set it to Bootstrap as that's more convenient for
// the user.
KDF11_A::KDF11_A (Qbus* bus)
    :
    bus_ (bus),
    powerUpMode_ {KD11Config::PowerUpMode::Bootstrap},
    startAddress_ {stdBootAddress}
{
    // Add the MMU to the CPU modules
    cpuModules_.push_back (&mmu_);

    // Besides a pointer to the bus, a reference to our cpu, the start address
    // and the power-up mode, the ControlLogic also gets passed a
    // std::function to the function to create ODT objects.
    controlLogic_ = make_unique<ControlLogic> (bus_, &cpuData_, &cpuControl_, &mmu_, powerUpMode_,
        startAddress_, bind (&KDF11_A_ODT::createODT, _1, _2, _3, _4, _5));
}

KDF11_A::KDF11_A (Qbus *bus, shared_ptr<KDF11_AConfig> kdf11_aConfig)
    :
    bus_ (bus),
    powerUpMode_ {kdf11_aConfig->powerUpMode},
    startAddress_ {kdf11_aConfig->startingAddress}
{
    // If the KTF11-A is configured add it to the CPU modules. If it is not
    // configured its registers will not be available on the bus and
    // consequently it cannot be enabled.
    if (kdf11_aConfig->ktf11_a_present)
        cpuModules_.push_back (&mmu_);

    // Besides a pointer to the bus, a reference to our cpu, the start address
    // and the power-up mode, the ControlLogic also gets passed a
    // std::function to the function to create ODT objects.
    controlLogic_ = make_unique<ControlLogic> (bus_, &cpuData_, &cpuControl_, &mmu_, powerUpMode_,
        startAddress_, bind (&KDF11_A_ODT::createODT, _1, _2, _3, _4, _5));
}

KDF11_A::~KDF11_A ()
{
    controlLogic_->exit ();
    kd11Thread_.join ();
}

void KDF11_A::start ()
{
    kd11Thread_ = thread ([&, this] {controlLogic_->run ();});
}

// Start the ControlLogic state machine, starting the CPU at the given address. This
// address supersedes the standard boot address.
void KDF11_A::start (u16 startAddress)
{
    startAddress_ = startAddress;
    start ();
}

StatusCode KDF11_A::read (BusAddress address, u16* destination)
{
    for (BusDevice* module : cpuModules_)
        if (module->responsible (address))
            return module->read (address, destination);

    return StatusCode::NonExistingMemory;
}

StatusCode KDF11_A::writeWord (BusAddress address, u16 value)
{
    for (BusDevice* module : cpuModules_)
        if (module->responsible (address))
            return module->writeWord (address, value);

    return StatusCode::NonExistingMemory;

}

bool KDF11_A::responsible (BusAddress busAddress)
{
    if (!busAddress.isInIOpage ())
        return false;

    for (BusDevice* module : cpuModules_)
        if (module->responsible (busAddress))
            return true;

    return false;
}

void KDF11_A::reset ()
{
    for (BusDevice* module : cpuModules_)
        module->reset ();
}   
