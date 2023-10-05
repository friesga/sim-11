#include "kd11_na.h"
#include "qbus/qbus.h"

#include <memory>
#include <thread>

using std::shared_ptr;
using std::make_unique;
using std::thread;

// The factory power-up mode configuration is mode 0 (get vector at address
// 24 and 26), but we'll set it to Bootstrap as that's more convenient for
// the user.
KD11_NA::KD11_NA (Qbus* bus)
    :
    bus_ (bus),
    powerUpMode_ {KD11_NAConfig::PowerUpMode::Bootstrap},
    startAddress_ {stdBootAddress}
{}

KD11_NA::KD11_NA (Qbus *bus, shared_ptr<KD11_NAConfig> kd11_naConfig)
    :
    KD11_NA (bus)
{
    powerUpMode_ = kd11_naConfig->powerUpMode;
}

KD11_NA::~KD11_NA ()
{
    kd11Thread_.join ();
}

KD11_NA_Cpu& KD11_NA::cpu ()
{
    return cpu_;
}

void KD11_NA::start ()
{
    kd11Thread_ = thread ([&, this] {controlLogic_.run ();});
}

// Start the ControlLogic state machine, starting the CPU at the given address. This
// address supersedes the standard boot address.
void KD11_NA::start (u16 startAddress)
{
    startAddress_ = startAddress;
    start ();
}
