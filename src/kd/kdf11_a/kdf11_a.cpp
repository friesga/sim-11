#include "kdf11_a.h"
#include "qbus/qbus.h"

#include <memory>
#include <thread>

using std::shared_ptr;
using std::make_unique;
using std::thread;

// The factory power-up mode configuration is mode 0 (get vector at address
// 24 and 26), but we'll set it to Bootstrap as that's more convenient for
// the user.
KDF11_A::KDF11_A (Qbus* bus)
    :
    bus_ (bus),
    powerUpMode_ {KDF11_AConfig::PowerUpMode::Bootstrap},
    startAddress_ {stdBootAddress}
{}

KDF11_A::KDF11_A (Qbus *bus, shared_ptr<KDF11_AConfig> kdf11_aConfig)
    :
    KDF11_A (bus)
{
    powerUpMode_ = kdf11_aConfig->powerUpMode;
}

KDF11_A::~KDF11_A ()
{
    controlLogic_.exit ();
    kd11Thread_.join ();
}

KD11_NA_Cpu& KDF11_A::cpu ()
{
    return cpu_;
}

void KDF11_A::start ()
{
    kd11Thread_ = thread ([&, this] {controlLogic_.run ();});
}

// Start the ControlLogic state machine, starting the CPU at the given address. This
// address supersedes the standard boot address.
void KDF11_A::start (u16 startAddress)
{
    startAddress_ = startAddress;
    start ();
}
