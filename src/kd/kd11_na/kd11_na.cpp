#include "kd11_na.h"
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
KD11_NA::KD11_NA (Bus* bus)
    :
    bus_ (bus),
    powerUpMode_ {KD11Config::PowerUpMode::Bootstrap},
    startAddress_ {stdBootAddress}
{
    // Besides a pointer to the bus, a reference to our cpu, the start address
    // and the power-up mode, the ControlLogic also gets passed a
    // std::function to the function to create ODT objects.
    controlLogic_ = make_unique<ControlLogic> (bus_, &cpuData_, &cpuControl_, &pseudoMMU_, powerUpMode_,
        startAddress_, bind (&KD11_NA_ODT::createODT, _1, _2, _3, _4, _5));
}

KD11_NA::KD11_NA (Bus *bus, shared_ptr<KD11_NAConfig> kd11_naConfig)
    :
    bus_ (bus),
    powerUpMode_ {kd11_naConfig->powerUpMode},
    startAddress_ {stdBootAddress}
{
    controlLogic_ = make_unique<ControlLogic> (bus_, &cpuData_, &cpuControl_, &pseudoMMU_, powerUpMode_,
        startAddress_, bind (&KD11_NA_ODT::createODT, _1, _2, _3, _4, _5));
}

KD11_NA::~KD11_NA ()
{
    controlLogic_->exit ();
    kd11Thread_.join ();
}

void KD11_NA::start ()
{
    kd11Thread_ = thread ([&, this] {controlLogic_->run ();});
}

// Start the ControlLogic state machine, starting the CPU at the given address. This
// address supersedes the standard boot address.
void KD11_NA::start (u16 startAddress)
{
    startAddress_ = startAddress;
    start ();
}
